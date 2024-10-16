#include <bringauto/communication/fleet_protocol/FleetProtocol.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>
#include <bringauto/communication/fleet_protocol/Message.hpp>

#include <bringauto/settings/LoggerId.hpp>

#include <nlohmann/json.hpp>


namespace bringauto::communication::fleet_protocol {

using json = nlohmann::ordered_json;

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

	settings::Logger::logInfo("Initializing connection to module gateway on {}:{}", globalContext_->settings->moduleGatewayIp,
							  globalContext_->settings->moduleGatewayPort);

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
		settings::Logger::logError("Failed to connect to module gateway, err code: {}", retCode);
		return false;
	}

	settings::Logger::logInfo("Connected to module gateway");
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

	if(!receiveCommand()) {
		isConnected_ = false;
		return false;
	}
	return true;
}

bool FleetProtocol::sendStatus(const Status &status) {
	auto statusString = generateCarStatusString(status);
	Message message(statusString);

	int retCode = send_status(internalClientContext_, message.getBuffer(), 60);
	if(retCode != OK) {
		destroy_connection(&internalClientContext_);
		settings::Logger::logError("Failed to send status to module gateway, err code: {}", retCode);
		return false;
	}
	return true;
}

bool FleetProtocol::receiveCommand() {
	buffer command_buffer { nullptr, 0 };

	int retCode = get_command(internalClientContext_, &command_buffer);
	if(retCode != OK) {
		destroy_connection(&internalClientContext_);
		settings::Logger::logError("Failed to get command from module gateway, err code: {}", retCode);
		return false;
	}

	processBufferData(command_buffer);
	deallocate(&command_buffer);
	return true;
}

void FleetProtocol::processBufferData(const buffer &bufferData) {
	Command command;
	json commandJson;
	char *data = static_cast<char*>(bufferData.data);

	try {
		commandJson = json::parse(data, data + bufferData.size_in_bytes);
		settings::Logger::logInfo("Parsing command: {}", commandJson.dump());
	} catch (json::parse_error &) {
		settings::Logger::logWarning("Cannot parse car command!");
		return;
	}

	std::vector<osm::Route::Station> missions;

	try {
		for(int i = 0; i < commandJson.at("stops").size(); i++) {
			osm::Route::Station station;
			station.name = commandJson.at("stops").at(i).at("name");
			station.latitude = commandJson.at("stops").at(i).at("position").at("latitude");
			station.longitude = commandJson.at("stops").at(i).at("position").at("longitude");
			missions.push_back(station);
		}
		command.setMission(missions);
		command.setRoute(commandJson.at("route"));
		command.setAction(common_utils::EnumUtils::valueToEnum<EAutonomyAction>(std::string(commandJson.at("action"))));
	} catch (json::exception &) {
		settings::Logger::logWarning("Cannot parse car command!");
		return;
	}

	if(currentCommand_ != command) {
		currentCommand_ = command;
		std::stringstream is;
		is << currentCommand_;
		settings::Logger::logInfo("Received new command: {}", is.str());
	}
}

std::string FleetProtocol::generateCarStatusString(const Status &status) {
	json statusJson;
	statusJson["telemetry"]["position"]["altitude"] = 0;
	statusJson["telemetry"]["position"]["latitude"] = status.getLatitude();
	statusJson["telemetry"]["position"]["longitude"] = status.getLongitude();
	statusJson["telemetry"]["speed"] = status.getSpeed();
	statusJson["telemetry"]["fuel"] = 0.42;
	statusJson["nextStop"]["name"] = status.getNextStop().name;
	statusJson["nextStop"]["position"]["altitude"] = 0;
	statusJson["nextStop"]["position"]["latitude"] = status.getNextStop().latitude;
	statusJson["nextStop"]["position"]["longitude"] = status.getNextStop().longitude;
	statusJson["state"] = common_utils::EnumUtils::enumToString<EAutonomyState>(status.getState());
	settings::Logger::logInfo("Generating car status: {}", statusJson.dump());
	return statusJson.dump();
}

FleetProtocol::~FleetProtocol() {
	destroy_connection(&internalClientContext_);
}

}

