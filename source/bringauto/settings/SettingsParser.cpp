#include <bringauto/settings/SettingsParser.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>
#include <bringauto/settings/Constants.hpp>

#include <iostream>
#include <fstream>



namespace bringauto::settings {

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
	options.add_options("general")("c, " + configPath_, "Path to configuration file", cxxopts::value<std::string>());
	options.add_options("general")(logPath_, "Path to logs", cxxopts::value<std::string>());
	options.add_options("general")("v, " + verbose_, "Print log messages into terminal");
	options.add_options("general")(statusMessagePeriod_, "Period in ms for sending status messages",
								   cxxopts::value<uint32_t>());
	options.add_options("vehicle")(vehicleProvider_,
								   R"(Choose virtual vehicle location provider, "simulation" or "gps".)",
								   cxxopts::value<std::string>());
	options.add_options("gps vehicle provider")(gpsProvider_, R"(Choose gps provider, "rutx09" or "ublox".)",
												cxxopts::value<std::string>());
	options.add_options("gps vehicle provider")(rutxIp_, "Modbus server address for rutx09.",
												cxxopts::value<std::string>());
	options.add_options("gps vehicle provider")(rutxPort_, "Modbus server port for rutx09.",
												cxxopts::value<int>());
	options.add_options("gps vehicle provider")(rutxSlaveId_, "Modbus server slave id for rutx09.",
												cxxopts::value<int>());
	options.add_options("gps vehicle provider")(stopRadius_, "Radius from stop for marking it done.",
												cxxopts::value<int>());
	options.add_options("simulation vehicle provider")(osmSpeedOverride_, "Override map speed on all points, in m/s",
													   cxxopts::value<uint32_t>());
	options.add_options("simulation vehicle provider")(osmStopWaitTime_,
													   "Wait time in stops in seconds, default is 10s",
													   cxxopts::value<uint32_t>());
	options.add_options("fleet")(fleetProvider_,
								 R"(Provider of communication with fleet, "protobuf" or "empty")",
								 cxxopts::value<std::string>());
	options.add_options("protobuf fleet provider")(daemonIp_, "IPv4 address or hostname of server side application",
												   cxxopts::value<std::string>());
	options.add_options("protobuf fleet provider")(daemonPort_, "Port of server side application",
												   cxxopts::value<int>());
	options.add_options("map")(osmMap_, "Path to .osm map file", cxxopts::value<std::string>());
	options.add_options("map")(osmRoute_, "Name of route that will be set on initialization",
							   cxxopts::value<std::string>());
	options.add_options()("h, " + help_, "Print usage");

	cmdArguments_ = options.parse(argc, argv);

	if(cmdArguments_.count("help") || argc == 1) {
		std::cout << options.help() << std::endl;
	}
}

bool SettingsParser::areCmdArgumentsCorrect() {
	bool isCorrect = true;
	std::vector<std::string> requiredParams {
			configPath_
	};
	std::vector<std::string> allParameters = { configPath_,
											   verbose_,
											   osmMap_,
											   osmRoute_,
											   logPath_,
											   daemonIp_,
											   daemonPort_,
											   osmStopWaitTime_,
											   statusMessagePeriod_,
											   osmSpeedOverride_,
											   vehicleProvider_,
											   gpsProvider_,
											   rutxIp_,
											   rutxPort_,
											   rutxSlaveId_,
											   fleetProvider_,
											   stopRadius_
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

	if(settings_->vehicleProvider == VehicleProvider::INVALID) {
		std::cerr << "Invalid vehicle provider\n";
		isCorrect = false;
	}

	if(!std::filesystem::exists(settings_->logPath)) {
		std::cerr << "Given log path (" << settings_->logPath << ") does not exist." << std::endl;
		isCorrect = false;
	}

	if(settings_->vehicleProvider == VehicleProvider::SIMULATION) {
		if(!std::filesystem::exists(settings_->mapFilePath)) {
			std::cerr << "Given map path (" << settings_->mapFilePath << ") does not exist." << std::endl;
			isCorrect = false;
		}
	}
	if(settings_->vehicleProvider == VehicleProvider::GPS) {
		if(settings_->gpsProvider == GpsProvider::INVALID) {
			std::cerr << "Invalid gps provider\n";
			isCorrect = false;
		}
	}
	if(settings_->fleetProvider == FleetProvider::INVALID) {
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

	auto configPath = cmdArguments_[configPath_].as<std::string>();
	std::ifstream inputFile(configPath);
	auto file = nlohmann::json::parse(inputFile);

	fillGeneralSettings(file);
	fillVehicleSettings(file);
	fillFleetSettings(file);
	fillMapSettings(file);
}

void SettingsParser::fillGeneralSettings(const nlohmann::json &file) {
	if(cmdArguments_.count(logPath_)) {
		settings_->logPath = cmdArguments_[logPath_].as<std::string>();
	} else {
		settings_->logPath = std::filesystem::path(file[generalSettings_][logPath_]);
	}
	if(cmdArguments_.count(verbose_)) {
		settings_->verbose = cmdArguments_.count(verbose_) == 1;
	} else {
		settings_->verbose = file[generalSettings_][verbose_];
	}
	if(cmdArguments_.count(statusMessagePeriod_)) {
		settings_->messagePeriodMs = cmdArguments_[statusMessagePeriod_].as<uint32_t>();
	} else {
		settings_->messagePeriodMs = file[generalSettings_][statusMessagePeriod_];
	}
}

void SettingsParser::fillVehicleSettings(const nlohmann::json &file) {
	if(cmdArguments_.count(vehicleProvider_)) {
		settings_->vehicleProvider = common_utils::EnumUtils::stringToVehicleProvider(
				cmdArguments_[vehicleProvider_].as<std::string>());
	} else {
		settings_->vehicleProvider = common_utils::EnumUtils::stringToVehicleProvider(
				file[vehicleSettings_][vehicleProvider_]);
	}
	if(settings_->vehicleProvider == VehicleProvider::GPS) {
		fillGpsSettings(file);
	}
	if(settings_->vehicleProvider == VehicleProvider::SIMULATION) {
		fillSimulationSettings(file);
	}
}

void SettingsParser::fillGpsSettings(const nlohmann::json &file) {
	if(cmdArguments_.count(gpsProvider_)) {
		settings_->gpsProvider = common_utils::EnumUtils::stringToGpsProvider(
				cmdArguments_[gpsProvider_].as<std::string>());
	} else {
		settings_->gpsProvider = common_utils::EnumUtils::stringToGpsProvider(
				file[vehicleSettings_][gpsSettings_][gpsProvider_]);

	}
	if(cmdArguments_.count(stopRadius_)) {
		settings_->stopRadius = cmdArguments_[stopRadius_].as<int>();
	} else {
		settings_->stopRadius = file[vehicleSettings_][gpsSettings_][stopRadius_];
	}
	if(settings_->gpsProvider == GpsProvider::RUTX09) {
		fillRutx09Settings(file);
	}
}

void SettingsParser::fillRutx09Settings(const nlohmann::json &file) {
		if(cmdArguments_.count(rutxIp_)) {
			settings_->rutxIp = cmdArguments_[rutxIp_].as<std::string>();
		} else {
			settings_->rutxIp = file[vehicleSettings_][gpsSettings_][rutx09Settings_][rutxIp_];
		}
		if(cmdArguments_.count(rutxPort_)) {
			settings_->rutxPort = cmdArguments_[rutxPort_].as<int>();
		} else {
			settings_->rutxPort = file[vehicleSettings_][gpsSettings_][rutx09Settings_][rutxPort_];
		}
		if(cmdArguments_.count(rutxSlaveId_)) {
			settings_->rutxSlaveId = cmdArguments_[rutxSlaveId_].as<int>();
		} else {
			settings_->rutxSlaveId = file[vehicleSettings_][gpsSettings_][rutx09Settings_][rutxSlaveId_];
		}
}

void SettingsParser::fillSimulationSettings(const nlohmann::json &file) {
	if(cmdArguments_.count(osmStopWaitTime_)) {
		settings_->stopWaitTime = cmdArguments_[osmStopWaitTime_].as<uint32_t>();
	} else {
		settings_->stopWaitTime = file[vehicleSettings_][simulationSettings_][osmStopWaitTime_];
	}

	if(cmdArguments_.count(osmSpeedOverride_)) {
		settings_->speedOverride = true;
		settings_->speedOverrideMS = cmdArguments_[osmSpeedOverride_].as<uint32_t>();
	} else {
		settings_->speedOverride = file[vehicleSettings_][simulationSettings_][osmSpeedOverride_];
		settings_->speedOverrideMS = file[vehicleSettings_][simulationSettings_][osmSpeedOverrideMs_];
	}
}

void SettingsParser::fillFleetSettings(const nlohmann::json &file) {
	if(cmdArguments_.count(fleetProvider_)) {
		settings_->fleetProvider = common_utils::EnumUtils::stringToFleetProvider(
				cmdArguments_[fleetProvider_].as<std::string>());
	} else {
		settings_->fleetProvider = common_utils::EnumUtils::stringToFleetProvider(file[fleetSettings_][fleetProvider_]);
	}
	if(settings_->fleetProvider == FleetProvider::PROTOBUF) {
		fillProtobufSettings(file);
	}
}

void SettingsParser::fillProtobufSettings(const nlohmann::json &file) {
		if(cmdArguments_.count(daemonIp_)) {
			settings_->daemonIpAddress = cmdArguments_[daemonIp_].as<std::string>();
		} else {
			settings_->daemonIpAddress = file[fleetSettings_][protobufSettings_][daemonIp_];
		}
		if(cmdArguments_.count(daemonPort_)) {
			settings_->daemonPort = cmdArguments_[daemonPort_].as<int>();
		} else {
			settings_->daemonPort = file[fleetSettings_][protobufSettings_][daemonPort_];
		}
}

void SettingsParser::fillMapSettings(const nlohmann::json &file) {
	if(cmdArguments_.count(osmMap_)) {
		settings_->mapFilePath = cmdArguments_[osmMap_].as<std::string>();
	} else {
		settings_->mapFilePath = std::filesystem::path(file[mapSettings_][osmMap_]);
	}
	if(cmdArguments_.count(osmRoute_)) {
		settings_->routeName = cmdArguments_[osmRoute_].as<std::string>();
	} else {
		settings_->routeName = file[mapSettings_][osmRoute_];
	}
}

std::string SettingsParser::getFormattedSettings() {
	std::stringstream formattedSettings;
	formattedSettings << "config-file: " << settings_->config << "\n";
	formattedSettings << "verbose: " << (settings_->verbose? "TRUE": "FALSE") << "\n";
	formattedSettings << "log-path: " << settings_->logPath << "\n";
	formattedSettings << "period-ms: " << settings_->messagePeriodMs << "\n";
	switch(settings_->fleetProvider) {
		case FleetProvider::INVALID:
			formattedSettings << "fleet-provider: INVALID\n";
			break;
		case FleetProvider::PROTOBUF:
			formattedSettings << "fleet-providerr: PROTOBUF\n";
			formattedSettings << "\tdaemon-ip: " << settings_->daemonIpAddress << "\n";
			formattedSettings << "\tdaemon-port: " << settings_->daemonPort << "\n";
			break;
		case FleetProvider::NO_CONNECTION:
			formattedSettings << "Fleet provider: EMPTY\n";
			break;
	}
	switch(settings_->vehicleProvider){
		case VehicleProvider::INVALID:
			formattedSettings << "vehicle-provider: INVALID\n";
			break;
		case VehicleProvider::SIMULATION:
			formattedSettings << "vehicle-provider: SIMULATION\n";
			if(settings_->speedOverride){
				formattedSettings << "\tspeed-override: TRUE\n";
				formattedSettings << "\tspeed-override-mps: " << settings_->speedOverrideMS << "\n";
			}else{
				formattedSettings << "\tspeed-override: FALSE\n";
			}
			formattedSettings << "\twait-at-stop-s: " << settings_->stopWaitTime << "\n";
			break;
		case VehicleProvider::GPS:
			formattedSettings << "vehicle-provider: GPS\n";
			switch(settings_->gpsProvider) {
				case GpsProvider::INVALID:
					formattedSettings << "\tgps-provider: INVALID\n";
					break;
				case GpsProvider::RUTX09:
					formattedSettings << "\tgps-provider: RUTX09\n";
					formattedSettings << "\trutx-ip: " << settings_->rutxIp << "\n";
					formattedSettings << "\trutx-port: " << settings_->rutxPort << "\n";
					formattedSettings << "\trutx-slave-id: " << settings_->rutxSlaveId << "\n";
					break;
				case GpsProvider::UBLOX:
					formattedSettings << "\tgps-provider: UBLOX\n";
					break;
				case GpsProvider::MAP:
					formattedSettings << "\tgps-provider: MAP\n";
					break;
			}
			break;
	}
	formattedSettings << "map: " << settings_->mapFilePath << "\n";
	formattedSettings << "default-route: " << (settings_->routeName.empty()? "\"\"": settings_->routeName);

	return formattedSettings.str();
}

}

