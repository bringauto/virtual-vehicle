#include <bringauto/settings/SettingsParser.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>

#include <iostream>
#include <fstream>



namespace bringauto::settings {

const std::string SettingsParser::CONFIG_PATH { "config" };
const std::string SettingsParser::VERBOSE { "verbose" };
const std::string SettingsParser::LOG_PATH { "log-path" };
const std::string SettingsParser::MODULE_GATEWAY_IP { "module-gateway-ip" };
const std::string SettingsParser::MODULE_GATEWAY_PORT { "module-gateway-port" };
const std::string SettingsParser::STATUS_MESSAGE_PERIOD { "period-ms" };
const std::string SettingsParser::VEHICLE_PROVIDER { "vehicle-provider" };
const std::string SettingsParser::FLEET_PROVIDER { "fleet-provider" };
const std::string SettingsParser::HELP { "help" };

const std::string SettingsParser::OSM_MAP { "map" };
const std::string SettingsParser::OSM_ROUTE { "default-route" };
const std::string SettingsParser::OSM_STOP_WAIT_TIME { "wait-at-stop-s" };
const std::string SettingsParser::OSM_SPEED_OVERRIDE { "speed-override" };
const std::string SettingsParser::OSM_SPEED_OVERRIDE_MPS { "speed-override-mps" };

const std::string SettingsParser::GPS_PROVIDER { "gps-provider" };
const std::string SettingsParser::RUT_IP { "rutx-ip" };
const std::string SettingsParser::RUT_PORT { "rutx-port" };
const std::string SettingsParser::RUT_SLAVE_ID { "rutx-slave-id" };
const std::string SettingsParser::STOP_RADIUS { "stop-radius-m" };
const std::string SettingsParser::DEVICE_NAME { "device-name" };
const std::string SettingsParser::DEVICE_ROLE { "device-role" };
const std::string SettingsParser::DEVICE_PRIORITY { "device-priority" };
const std::string SettingsParser::RECONNECT_PERIOD { "reconnect-period-s" };

const std::string SettingsParser::GENERAL_SETTINGS { "general-settings" };
const std::string SettingsParser::VEHICLE_SETTINGS { "vehicle-settings" };
const std::string SettingsParser::GPS_SETTINGS { "gps-settings" };
const std::string SettingsParser::RUTX_09_SETTINGS { "rutx09-settings" };
const std::string SettingsParser::SIMULATION_SETTINGS { "simulation-settings" };
const std::string SettingsParser::FLEET_SETTINGS { "fleet-settings" };
const std::string SettingsParser::INTERNAL_PROTOCOL_SETTINGS { "internal-protocol-settings" };
const std::string SettingsParser::MAP_SETTINGS { "map-settings" };

bool SettingsParser::parseSettings(int argc, char **argv) {
	parseCmdArguments(argc, argv);
	if(cmdArguments_.count("help") || argc == 1) {
		return false;
	}

	if(!areCmdArgumentsCorrect()) {
		throw std::invalid_argument("Cmd arguments are not correct");
	}

	fillSettings();

	if(!areSettingsCorrect()) {
		throw std::invalid_argument("Arguments are not correct.");
	}

	return true;
}

void SettingsParser::parseCmdArguments(int argc, char **argv) {
	cxxopts::Options options { "VirtualVehicle", "BringAuto virtual vehicle utility" };
	options.add_options("general")("c, " + CONFIG_PATH, "Path to configuration file", cxxopts::value<std::string>());
	options.add_options("general")(LOG_PATH, "Path to logs", cxxopts::value<std::string>());
	options.add_options("general")("v, " + VERBOSE, "Print log messages into terminal");
	options.add_options("general")(STATUS_MESSAGE_PERIOD, "Period in ms for sending status messages",
								   cxxopts::value<uint32_t>());
	options.add_options("vehicle")(VEHICLE_PROVIDER,
								   R"(Choose virtual vehicle location provider, "simulation" or "gps".)",
								   cxxopts::value<std::string>());
	options.add_options("gps vehicle provider")(GPS_PROVIDER, R"(Choose gps provider, "rutx09" or "ublox".)",
												cxxopts::value<std::string>());
	options.add_options("gps vehicle provider")(RUT_IP, "Modbus server address for rutx09.",
												cxxopts::value<std::string>());
	options.add_options("gps vehicle provider")(RUT_PORT, "Modbus server port for rutx09.",
												cxxopts::value<int>());
	options.add_options("gps vehicle provider")(RUT_SLAVE_ID, "Modbus server slave id for rutx09.",
												cxxopts::value<int>());
	options.add_options("gps vehicle provider")(STOP_RADIUS, "Radius from stop for marking it done.",
												cxxopts::value<int>());
	options.add_options("simulation vehicle provider")(OSM_SPEED_OVERRIDE, "Override map speed on all points, in m/s",
													   cxxopts::value<uint32_t>());
	options.add_options("simulation vehicle provider")(OSM_STOP_WAIT_TIME,
													   "Wait time in stops in seconds, default is 10s",
													   cxxopts::value<uint32_t>());
	options.add_options("fleet")(FLEET_PROVIDER,
								 R"(Provider of communication with fleet, "protobuf" or "empty")",
								 cxxopts::value<std::string>());
	options.add_options("protobuf fleet provider")(MODULE_GATEWAY_IP,
												   "IPv4 address or hostname of server side application",
												   cxxopts::value<std::string>());
	options.add_options("protobuf fleet provider")(MODULE_GATEWAY_PORT, "Port of server side application",
												   cxxopts::value<int>());
	options.add_options("map")(OSM_MAP, "Path to .osm map file", cxxopts::value<std::string>());
	options.add_options("map")(OSM_ROUTE, "Name of route that will be set on initialization",
							   cxxopts::value<std::string>());
	options.add_options()("h, " + HELP, "Print usage");

	cmdArguments_ = options.parse(argc, argv);

	if(cmdArguments_.count("help") || argc == 1) {
		std::cout << options.help() << std::endl;
	}
}

bool SettingsParser::areCmdArgumentsCorrect() {
	bool isCorrect = true;
	std::vector<std::string> requiredParams {
			CONFIG_PATH
	};
	std::vector<std::string> allParameters = { CONFIG_PATH,
											   VERBOSE,
											   OSM_MAP,
											   OSM_ROUTE,
											   LOG_PATH,
											   MODULE_GATEWAY_IP,
											   MODULE_GATEWAY_PORT,
											   OSM_STOP_WAIT_TIME,
											   STATUS_MESSAGE_PERIOD,
											   OSM_SPEED_OVERRIDE,
											   VEHICLE_PROVIDER,
											   GPS_PROVIDER,
											   RUT_IP,
											   RUT_PORT,
											   RUT_SLAVE_ID,
											   FLEET_PROVIDER,
											   STOP_RADIUS
	};
	allParameters.insert(allParameters.end(), requiredParams.begin(), requiredParams.end());


	for(const auto &param: allParameters) {
		if(cmdArguments_.count(param) > 1) {
			isCorrect = false;

			std::cerr << "[ERROR] Found duplicate --" << param << " cmdline parameter!" << std::endl;
		}
	}

	if(!cmdArguments_.unmatched().empty()) {
		std::string errorMessage = "Unmatched arguments! use -h to see arguments, arguments should be in format -<short name> <value> or --<long name>=<value>, unmatched arguments: ";
		for(const auto &param: cmdArguments_.unmatched()) {
			errorMessage += param + " ";
		}
		std::cerr << "[ERROR] " << errorMessage << std::endl;
		isCorrect = false;
	}

	for(const auto &param: requiredParams) {
		if(cmdArguments_.count(param) < 1) {
			isCorrect = false;
			std::cerr << "Please provide " << param << " argument" << std::endl;
		}
	}

	return isCorrect;
}

bool SettingsParser::areSettingsCorrect() {
	bool isCorrect = true;

	if(settings_->vehicleProvider == VehicleProvider::E_INVALID) {
		std::cerr << "Invalid vehicle provider\n";
		isCorrect = false;
	}

	if(!std::filesystem::exists(settings_->logPath)) {
		std::cerr << "Given log path (" << settings_->logPath << ") does not exist." << std::endl;
		isCorrect = false;
	}

	if(settings_->vehicleProvider == VehicleProvider::E_SIMULATION) {
		if(!std::filesystem::exists(settings_->mapFilePath)) {
			std::cerr << "Given map path (" << settings_->mapFilePath << ") does not exist." << std::endl;
			isCorrect = false;
		}
	}
	if(settings_->vehicleProvider == VehicleProvider::E_GPS) {
		if(settings_->gpsProvider == GpsProvider::E_INVALID) {
			std::cerr << "Invalid gps provider\n";
			isCorrect = false;
		}
	}
	if(settings_->fleetProvider == FleetProvider::E_INVALID) {
		std::cerr << "Invalid fleet provider\n";
		isCorrect = false;
	}

	return isCorrect;
}

std::shared_ptr<Settings> SettingsParser::getSettings() {
	return settings_;
}

void SettingsParser::fillSettings() {
	settings_ = std::make_shared<Settings>();

	auto configPath = cmdArguments_[CONFIG_PATH].as<std::string>();
	std::ifstream inputFile(configPath);
	auto file = nlohmann::json::parse(inputFile);

	fillGeneralSettings(file[GENERAL_SETTINGS]);
	fillVehicleSettings(file[VEHICLE_SETTINGS]);
	fillFleetSettings(file[FLEET_SETTINGS]);
	fillMapSettings(file[MAP_SETTINGS]);
}

void SettingsParser::fillGeneralSettings(const nlohmann::json &section) {
	if(cmdArguments_.count(LOG_PATH)) {
		settings_->logPath = cmdArguments_[LOG_PATH].as<std::string>();
	} else {
		settings_->logPath = std::filesystem::path(section[LOG_PATH]);
	}
	if(cmdArguments_.count(VERBOSE)) {
		settings_->verbose = cmdArguments_.count(VERBOSE) == 1;
	} else {
		settings_->verbose = section[VERBOSE];
	}
	if(cmdArguments_.count(STATUS_MESSAGE_PERIOD)) {
		settings_->messagePeriodMs = cmdArguments_[STATUS_MESSAGE_PERIOD].as<uint32_t>();
	} else {
		settings_->messagePeriodMs = section[STATUS_MESSAGE_PERIOD];
	}
}

void SettingsParser::fillVehicleSettings(const nlohmann::json &section) {
	if(cmdArguments_.count(VEHICLE_PROVIDER)) {
		settings_->vehicleProvider = common_utils::EnumUtils::valueToEnum<settings::VehicleProvider>(
				cmdArguments_[VEHICLE_PROVIDER].as<std::string>());
	} else {
		settings_->vehicleProvider = common_utils::EnumUtils::valueToEnum<settings::VehicleProvider, std::string>(
				section[VEHICLE_PROVIDER]);
	}
	if(settings_->vehicleProvider == VehicleProvider::E_GPS) {
		fillGpsSettings(section[GPS_SETTINGS]);
	}
	if(settings_->vehicleProvider == VehicleProvider::E_SIMULATION) {
		fillSimulationSettings(section[SIMULATION_SETTINGS]);
	}
}

void SettingsParser::fillGpsSettings(const nlohmann::json &section) {
	if(cmdArguments_.count(GPS_PROVIDER)) {
		settings_->gpsProvider = common_utils::EnumUtils::valueToEnum<settings::GpsProvider, std::string>(
				cmdArguments_[GPS_PROVIDER].as<std::string>());
	} else {
		settings_->gpsProvider = common_utils::EnumUtils::valueToEnum<settings::GpsProvider, std::string>(
				section[GPS_PROVIDER]);

	}
	if(cmdArguments_.count(STOP_RADIUS)) {
		settings_->stopRadius = cmdArguments_[STOP_RADIUS].as<int>();
	} else {
		settings_->stopRadius = section[STOP_RADIUS];
	}
	if(settings_->gpsProvider == GpsProvider::E_RUTX09) {
		fillRutx09Settings(section[RUTX_09_SETTINGS]);
	}
}

void SettingsParser::fillRutx09Settings(const nlohmann::json &section) {
	if(cmdArguments_.count(RUT_IP)) {
		settings_->rutxIp = cmdArguments_[RUT_IP].as<std::string>();
	} else {
		settings_->rutxIp = section[RUT_IP];
	}
	if(cmdArguments_.count(RUT_PORT)) {
		settings_->rutxPort = cmdArguments_[RUT_PORT].as<int>();
	} else {
		settings_->rutxPort = section[RUT_PORT];
	}
	if(cmdArguments_.count(RUT_SLAVE_ID)) {
		settings_->rutxSlaveId = cmdArguments_[RUT_SLAVE_ID].as<int>();
	} else {
		settings_->rutxSlaveId = section[RUT_SLAVE_ID];
	}
}

void SettingsParser::fillSimulationSettings(const nlohmann::json &section) {
	if(cmdArguments_.count(OSM_STOP_WAIT_TIME)) {
		settings_->stopWaitTime = cmdArguments_[OSM_STOP_WAIT_TIME].as<uint32_t>();
	} else {
		settings_->stopWaitTime = section[OSM_STOP_WAIT_TIME];
	}

	if(cmdArguments_.count(OSM_SPEED_OVERRIDE)) {
		settings_->speedOverride = true;
		settings_->speedOverrideMS = cmdArguments_[OSM_SPEED_OVERRIDE].as<uint32_t>();
	} else {
		settings_->speedOverride = section[OSM_SPEED_OVERRIDE];
		settings_->speedOverrideMS = section[OSM_SPEED_OVERRIDE_MPS];
	}
}

void SettingsParser::fillFleetSettings(const nlohmann::json &section) {
	if(cmdArguments_.count(FLEET_PROVIDER)) {
		settings_->fleetProvider = common_utils::EnumUtils::valueToEnum<settings::FleetProvider, std::string>(
				cmdArguments_[FLEET_PROVIDER].as<std::string>());
	} else {
		settings_->fleetProvider = common_utils::EnumUtils::valueToEnum<settings::FleetProvider, std::string>(
				section[FLEET_PROVIDER]);
	}
	if(settings_->fleetProvider == FleetProvider::E_INTERNAL_PROTOCOL) {
		fillInternalProtocolSettings(section[INTERNAL_PROTOCOL_SETTINGS]);
	}
}

void SettingsParser::fillInternalProtocolSettings(const nlohmann::json &section) {
	if(cmdArguments_.count(MODULE_GATEWAY_IP)) {
		settings_->moduleGatewayIp = cmdArguments_[MODULE_GATEWAY_IP].as<std::string>();
	} else {
		settings_->moduleGatewayIp = section[MODULE_GATEWAY_IP];
	}
	if(cmdArguments_.count(MODULE_GATEWAY_PORT)) {
		settings_->moduleGatewayPort = cmdArguments_[MODULE_GATEWAY_PORT].as<int>();
	} else {
		settings_->moduleGatewayPort = section[MODULE_GATEWAY_PORT];
	}
	settings_->deviceName = section[DEVICE_NAME];
	settings_->deviceRole = section[DEVICE_ROLE];
	settings_->devicePriority = section[DEVICE_PRIORITY];
	settings_->reconnectPeriodS = section[RECONNECT_PERIOD];
}

void SettingsParser::fillMapSettings(const nlohmann::json &section) {
	if(cmdArguments_.count(OSM_MAP)) {
		settings_->mapFilePath = cmdArguments_[OSM_MAP].as<std::string>();
	} else {
		settings_->mapFilePath = std::filesystem::path(section[OSM_MAP]);
	}
	if(cmdArguments_.count(OSM_ROUTE)) {
		settings_->routeName = cmdArguments_[OSM_ROUTE].as<std::string>();
	} else {
		settings_->routeName = section[OSM_ROUTE];
	}
}

std::string SettingsParser::getFormattedSettings() {
	std::stringstream formattedSettings;
	formattedSettings << "config-file: " << settings_->config << "\n";
	formattedSettings << "verbose: " << (settings_->verbose ? "TRUE" : "FALSE") << "\n";
	formattedSettings << "log-path: " << settings_->logPath << "\n";
	formattedSettings << "period-ms: " << settings_->messagePeriodMs << "\n";
	switch(settings_->fleetProvider) {
		case FleetProvider::E_INTERNAL_PROTOCOL:
			formattedSettings << "fleet-providerr: INTERNAL_PROTOCOL\n";
			formattedSettings << "\tmodule-gateway-ip: " << settings_->moduleGatewayIp << "\n";
			formattedSettings << "\tmodule-gateway-port: " << settings_->moduleGatewayPort << "\n";
			formattedSettings << "\tdevice-role: " << settings_->deviceRole << "\n";
			formattedSettings << "\tdevice-name: " << settings_->deviceName << "\n";
			formattedSettings << "\tdevice-priority: " << settings_->devicePriority << "\n";
			formattedSettings << "\treconnect-period-s: " << settings_->reconnectPeriodS << "\n";
			break;
		case FleetProvider::E_NO_CONNECTION:
			formattedSettings << "Fleet provider: NO-CONNECTION\n";
			break;
		default:
			formattedSettings << "fleet-provider: INVALID\n";
			break;
	}
	switch(settings_->vehicleProvider) {
		case VehicleProvider::E_SIMULATION:
			formattedSettings << "vehicle-provider: SIMULATION\n";
			if(settings_->speedOverride) {
				formattedSettings << "\tspeed-override: TRUE\n";
				formattedSettings << "\tspeed-override-mps: " << settings_->speedOverrideMS << "\n";
			} else {
				formattedSettings << "\tspeed-override: FALSE\n";
			}
			formattedSettings << "\twait-at-stop-s: " << settings_->stopWaitTime << "\n";
			break;
		case VehicleProvider::E_GPS:
			formattedSettings << "vehicle-provider: GPS\n";
			switch(settings_->gpsProvider) {
				case GpsProvider::E_RUTX09:
					formattedSettings << "\tgps-provider: RUTX09\n";
					formattedSettings << "\trutx-ip: " << settings_->rutxIp << "\n";
					formattedSettings << "\trutx-port: " << settings_->rutxPort << "\n";
					formattedSettings << "\trutx-slave-id: " << settings_->rutxSlaveId << "\n";
					break;
				case GpsProvider::E_UBLOX:
					formattedSettings << "\tgps-provider: UBLOX\n";
					break;
				case GpsProvider::E_MAP:
					formattedSettings << "\tgps-provider: MAP\n";
					break;
				default:
					formattedSettings << "\tgps-provider: INVALID\n";
					break;
			}
			break;
		default:
			formattedSettings << "vehicle-provider: INVALID\n";
			break;
	}
	formattedSettings << "map: " << settings_->mapFilePath << "\n";
	formattedSettings << "default-route: " << (settings_->routeName.empty() ? "\"\"" : settings_->routeName);

	return formattedSettings.str();
}

}

