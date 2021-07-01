#include <bringauto/communication/ProtoBuffer.hpp>
#include <bringauto/logging/Logger.hpp>
#include <BringAutoDaemon.pb.h>

void bringauto::communication::ProtoBuffer::initializeConnection() {
    using boost::asio::ip::tcp;
    if (socket_.is_open()) {
        socket_.close();
    }
    socket_.connect(tcp::endpoint(boost::asio::ip::address::from_string(ipAddress_), port_));
}

void bringauto::communication::ProtoBuffer::sendStatus(double lat, double lon, double speed) {
    using boost::asio::ip::tcp;

    BringAutoDaemon::CarStatus carStatus;
    auto telemetry = new BringAutoDaemon::CarStatus_Telemetry;
    auto position = new BringAutoDaemon::CarStatus_Position();
    carStatus.set_allocated_telemetry(telemetry);
    telemetry->set_speed(speed);
    position->set_longitude(lon);
    position->set_latitude(lat);

    telemetry->set_allocated_position(position);

    boost::system::error_code error;
    std::string buffer = carStatus.SerializeAsString();
    uint32_t messageSize = buffer.size();

    boost::asio::write(socket_, boost::asio::buffer(&messageSize, sizeof(uint32_t)), error);
    boost::asio::write(socket_, boost::asio::buffer(buffer, messageSize), error);
    if (error) {
        bringauto::logging::Logger::logError(
                "Sending to " + ipAddress_ + ":" + std::to_string(port_) + " failed: " + error.message());
        initializeConnection();
    }
}

bringauto::communication::ProtoBuffer::~ProtoBuffer() {
    if (socket_.is_open()) {
        socket_.close();
    }
    google::protobuf::ShutdownProtobufLibrary();
    bringauto::logging::Logger::logInfo("Closing proto buffer");
}
