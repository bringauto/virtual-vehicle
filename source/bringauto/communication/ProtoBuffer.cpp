#include <bringauto/communication/ProtoBuffer.hpp>
#include <bringauto/logging/Logger.hpp>



namespace bringauto::communication {
void ProtoBuffer::initializeConnection() {
	using boost::asio::ip::tcp;
	if(socket_.is_open()) {
		socket_.close();
	}

	boost::asio::io_service ios;
	boost::asio::ip::tcp::resolver::query resolver_query(globalContext_->settings->ipAddress,
														 std::to_string(globalContext_->settings->port),
														 boost::asio::ip::tcp::resolver::query::numeric_service);
	boost::asio::ip::tcp::resolver resolver(ios);
	boost::system::error_code ec;
	boost::asio::ip::tcp::resolver::iterator it =
			resolver.resolve(resolver_query, ec);
	if(ec.failed()) {
		globalContext_->ioContext.stop();
		throw std::runtime_error(
				"Failed to resolve a DNS name. Error code = " + std::to_string(ec.value()) + ". Message = " +
				ec.message());
	}

	socket_.connect(it->endpoint());
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::connectedState);
#endif

}

void ProtoBuffer::makeRequest(const Status &status) {
	sendStatus(status);
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::statusSentState);
#endif
	receiveCommand();
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::commandReceivedState);
#endif
	processBufferData();
}

void ProtoBuffer::sendStatus(const Status &status) {
	auto statusString = generateCarStatusString(status);
	uint32_t messageSize = statusString.size();
	socket_.write_some(boost::asio::buffer(&messageSize, sizeof(uint32_t)));
	socket_.write_some(boost::asio::buffer(statusString, messageSize));
}

void ProtoBuffer::receiveCommand() {
	auto bytesTransferred = socket_.read_some(boost::asio::buffer(receivedData_.buffer, headerSize_));

	if(bytesTransferred != headerSize_) {
		logging::Logger::logWarning(
				"Received header size does not match, received: {}, should be: {}",
				bytesTransferred, headerSize_);
		return;
	}

	std::memcpy(&receivedData_.commandMessageSize, receivedData_.buffer.data(), headerSize_);
	socket_.read_some(boost::asio::buffer(receivedData_.buffer, receivedData_.commandMessageSize));
}

void ProtoBuffer::processBufferData() {
	Command newCommand;
	BringAutoDaemon::CarCommand protobuffCommandMessage {};

	if(!protobuffCommandMessage.ParseFromArray(receivedData_.buffer.data(),
											   static_cast<int>(receivedData_.commandMessageSize))) {
		logging::Logger::logWarning("Cannot parse car status!");
		return;
	}

	newCommand.action = (Command::Action)protobuffCommandMessage.action();

	for(const auto &messageStop: protobuffCommandMessage.stops()) {
		newCommand.stops.push_back(messageStop.to());
	}


	if(command_ != newCommand) {
		command_ = newCommand;
		std::stringstream is;
		is << command_;
		logging::Logger::logInfo("Received new command: {}", is.str());
	}
}

std::string ProtoBuffer::generateCarStatusString(const Status &status) {
	BringAutoDaemon::CarStatus carStatus;

	auto position = new BringAutoDaemon::CarStatus_Position();
	position->set_longitude(status.longitude);
	position->set_latitude(status.latitude);

	auto telemetry = new BringAutoDaemon::CarStatus_Telemetry;
	telemetry->set_speed(status.speed);
	telemetry->set_fuel(0.42);
	telemetry->set_allocated_position(position);

	auto stop = new BringAutoDaemon::Stop();
	stop->set_to(status.nextStop);

	carStatus.set_state((BringAutoDaemon::CarStatus_State)status.state);
	carStatus.set_allocated_telemetry(telemetry);
	carStatus.set_allocated_stop(stop);
	return carStatus.SerializeAsString();
}

ProtoBuffer::~ProtoBuffer() {
	if(socket_.is_open()) {
		boost::system::error_code ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		socket_.cancel();
		socket_.close();
	}
	google::protobuf::ShutdownProtobufLibrary();
	bringauto::logging::Logger::logInfo("Closing proto buffer");
}
}

