#include <bringauto/communication/fleet_protocol/FleetProtocol.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>
#include <bringauto/communication/fleet_protocol/Message.hpp>

#include <bringauto/logging/Logger.hpp>



namespace bringauto::communication::fleet_protocol {
bool FleetProtocol::initializeConnection() {
	using boost::asio::ip::tcp;
	boost::asio::io_service ios;
	isConnected_ = false;

	auto tp = std::chrono::system_clock::now();
	auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

	if((currentTime - lastConnectAttemptS_) < globalContext_->settings->reconnectPeriodS) {
		return false;
	}
	lastConnectAttemptS_ = currentTime;


	boost::asio::ip::tcp::resolver::query resolver_query(globalContext_->settings->moduleGatewayIp,
														 std::to_string(globalContext_->settings->moduleGatewayPort),
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

	it->endpoint().address().to_string();

	buffer deviceName { nullptr, 0 };
	allocate(&deviceName, globalContext_->settings->deviceName.size());
	std::memcpy(deviceName.data, globalContext_->settings->deviceName.c_str(), deviceName.size_in_bytes);

	buffer deviceRole { nullptr, 0 };
	allocate(&deviceRole, globalContext_->settings->deviceRole.size());
	std::memcpy(deviceRole.data, globalContext_->settings->deviceRole.c_str(), deviceRole.size_in_bytes);

	struct device_identification deviceSettings {
			MISSION_MODULE_NUMBER,
			AUTONOMY_DEVICE_NUMBER,
			deviceRole,
			deviceName,
			globalContext_->settings->devicePriority
	};

	auto retCode = init_connection(&internalClientContext_,
								   it->endpoint().address().to_string().c_str(),
								   it->endpoint().port(),
								   deviceSettings);

	deallocate(&deviceName);
	deallocate(&deviceRole);

	if(retCode != OK) {
		destroy_connection(&internalClientContext_);
		logging::Logger::logError("Failed to connect to module gateway, err code: {}", retCode);
		return false;
	}

	logging::Logger::logInfo("Connected to module gateway");
	isConnected_ = true;

#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::connectedState);
#endif
	return true;
}

bool FleetProtocol::makeRequest(const Status &status) {
	if(!isConnected_) {
		return false;
	}

	if(!sendStatus(status)) {
		isConnected_ = false;
		return false;
	}
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::statusSentState);
#endif
	if(!receiveCommand()) {
		isConnected_ = false;
		return false;
	}

#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::commandReceivedState);
#endif
	return true;
}

bool FleetProtocol::sendStatus(const Status &status) {
	auto statusString = generateCarStatusString(status);
	Message message(statusString);

	int retCode = send_status(internalClientContext_, message.getBuffer(), 60);
	if(retCode != OK) {
		destroy_connection(&internalClientContext_);
		logging::Logger::logError("Failed to send status to module gateway, err code: {}", retCode);
		return false;
	}
	return true;
}

bool FleetProtocol::receiveCommand() {
	buffer command_buffer { nullptr, 0 };

	int retCode = get_command(internalClientContext_, &command_buffer);
	if(retCode != OK) {
		destroy_connection(&internalClientContext_);
		logging::Logger::logError("Failed to get command from module gateway, err code: {}", retCode);
		return false;
	}

	processBufferData(command_buffer);
	deallocate(&command_buffer);
	return true;
}

void FleetProtocol::processBufferData(const buffer &bufferData) {
	Command command;
	MissionModule::AutonomyCommand protoCommand {};

	if(!protoCommand.ParseFromArray(bufferData.data, bufferData.size_in_bytes)) {
		logging::Logger::logWarning("Cannot parse car command!");
		return;
	}

	command.setAction(common_utils::EnumUtils::valueToEnum<EAutonomyAction>(protoCommand.action()));

	std::vector<osm::Route::Station> missions;
	for(const auto &messageStop: protoCommand.stops()) {
		osm::Route::Station station;
		station.name = messageStop.name();
		station.latitude = messageStop.position().latitude();
		station.longitude = messageStop.position().longitude();
		missions.push_back(station);
	}
	command.setMission(missions);
	command.setRoute(protoCommand.route());

	if(currentCommand_ != command) {
		currentCommand_ = command;
		std::stringstream is;
		is << currentCommand_;
		logging::Logger::logInfo("Received new command: {}", is.str());
	}
}

std::string FleetProtocol::generateCarStatusString(const Status &status) {
	MissionModule::AutonomyStatus carStatus;

	auto position = new MissionModule::Position();
	position->set_longitude(status.getLongitude());
	position->set_latitude(status.getLatitude());

	auto telemetry = new MissionModule::AutonomyStatus_Telemetry();
	telemetry->set_speed(status.getSpeed());
	telemetry->set_fuel(0.42);
	telemetry->set_allocated_position(position);

	auto stop = new MissionModule::Station();
	stop->set_name(status.getNextStop().name);
	auto stopPosition = new MissionModule::Position();
	stopPosition->set_latitude(status.getNextStop().latitude);
	stopPosition->set_longitude(status.getNextStop().longitude);
	stop->set_allocated_position(stopPosition);

	carStatus.set_state(common_utils::EnumUtils::valueToEnum<MissionModule::AutonomyStatus_State>(status.getState()));
	carStatus.set_allocated_telemetry(telemetry);
	carStatus.set_allocated_nextstop(stop);
	return carStatus.SerializeAsString();
}

FleetProtocol::~FleetProtocol() {
	destroy_connection(&internalClientContext_);
	google::protobuf::ShutdownProtobufLibrary();
	bringauto::logging::Logger::logInfo("Closing proto buffer");
}

}

