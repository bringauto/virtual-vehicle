#include <bringauto/communication/ProtoBuffer.hpp>
#include <bringauto/logging/Logger.hpp>

void bringauto::communication::ProtoBuffer::initializeConnection() {
    using boost::asio::ip::tcp;
    if (socket_.is_open()) {
        socket_.close();
    }
    socket_.connect(tcp::endpoint(boost::asio::ip::address::from_string(ipAddress_), port_));

    addAsyncReceive();
}

void bringauto::communication::ProtoBuffer::sendStatus(double lat, double lon, double speed, State state, const std::string& nextStop) {
    using boost::asio::ip::tcp;

    BringAutoDaemon::CarStatus carStatus;
    auto telemetry = new BringAutoDaemon::CarStatus_Telemetry;
    auto position = new BringAutoDaemon::CarStatus_Position();
    auto stop = new BringAutoDaemon::Stop();
    telemetry->set_speed(speed);
    position->set_longitude(lon);
    position->set_latitude(lat);

    stop->set_to(nextStop);

    carStatus.set_allocated_telemetry(telemetry);
    carStatus.set_state((BringAutoDaemon::CarStatus_State)state);
    telemetry->set_allocated_position(position);
    carStatus.set_allocated_stop(stop);

    boost::system::error_code error;
    std::string buffer = carStatus.SerializeAsString();
    uint32_t messageSize = buffer.size();
    socket_.write_some(boost::asio::buffer(&messageSize, sizeof(uint32_t)));
    socket_.write_some(boost::asio::buffer(buffer, messageSize));
    if (error) {
        bringauto::logging::Logger::logError(
                "Sending to " + ipAddress_ + ":" + std::to_string(port_) + " failed: " + error.message());
        initializeConnection();
    }
}

bringauto::communication::ProtoBuffer::~ProtoBuffer() {
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket_.cancel();
        socket_.close();
    }
    google::protobuf::ShutdownProtobufLibrary();
    bringauto::logging::Logger::logInfo("Closing proto buffer");
}

const bringauto::communication::ProtoBuffer::Command &bringauto::communication::ProtoBuffer::getCommand() {
    isNewCommand_ = false;
    return command_;
}

void bringauto::communication::ProtoBuffer::addAsyncReceive() {
    socket_.async_read_some(
            boost::asio::buffer(receivedData_.buffer, headerSize_),
            [this](const boost::system::error_code &error, const std::size_t bytesTransferred) {
                asyncReceiveHandler(error, bytesTransferred);
            }
    );
}

void bringauto::communication::ProtoBuffer::asyncReceiveHandler(const boost::system::error_code &error,
                                                                const std::size_t bytesTransferred) {
    if (error) {
        logging::Logger::logWarning("Error occurred in proto command receive");
        if(!context_->ioContext.stopped()){
            addAsyncReceive();
        }
        return;
    }

    if (bytesTransferred != headerSize_) {
        logging::Logger::logWarning(
                "Received header size does not match, received: " + std::to_string(bytesTransferred) + ", should be: " +
                std::to_string(headerSize_));
        addAsyncReceive();
        return;
    }

    std::memcpy(&receivedData_.commandMessageSize, receivedData_.buffer.data(), headerSize_);
    socket_.read_some(boost::asio::buffer(receivedData_.buffer, receivedData_.commandMessageSize));
    processBufferData();
    addAsyncReceive();
}

void bringauto::communication::ProtoBuffer::processBufferData() {
    Command newCommand;

    BringAutoDaemon::CarCommand protobuffCommandMessage{};

    if (!protobuffCommandMessage.ParseFromArray(receivedData_.buffer.data(),
                                                static_cast<int>(receivedData_.commandMessageSize))) {
        logging::Logger::logWarning("Cannot parse car status!");
        return;
    }

    newCommand.action = (Command::Action) protobuffCommandMessage.action();

    for (const auto &messageStop: protobuffCommandMessage.stops()) {
        newCommand.stops.push_back(messageStop.to());
    }
    command_ = newCommand;
    isNewCommand_ = true;
}