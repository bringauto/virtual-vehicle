#include <bringauto/settings/SettingsParser.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>
#include <bringauto/settings/Constants.hpp>

#include <iostream>

namespace bringauto::settings {

bool SettingsParser::parseSettings(int argc, char **argv) {
	parseCmdArguments(argc, argv);

	if(cmdArguments_.count("help") || argc == 1) {
		return false;
	}

	if(!areCmdArgumentsCorrect()) {
		throw std::invalid_argument("Multiple arguments of type provided, please remove duplicate arguments.");
	}

	fillSettings();

	if(!areSettingsCorrect()) {
		throw std::invalid_argument("Arguments are not correct.");
	}

	return true;
}

void SettingsParser::parseCmdArguments(int argc, char **argv) {
	cxxopts::Options options { "VirtualVehicle", "BringAuto virtual vehicle utility" };
	options.add_options()(logPath_, "Path to logs", cxxopts::value<std::string>()->default_value("./"));
	options.add_options()("v, " + verbose_, "Print log messages into terminal");
	options.add_options()(statusMessagePeriod_, "Period in ms for sending status messages",
						  cxxopts::value<uint32_t>()->default_value("1000"));
	options.add_options("osm")(osmMap_, "Path to .osm map file", cxxopts::value<std::string>());
	options.add_options("osm")(osmRoute_, "Name of route that will be set on initialization",
										 cxxopts::value<std::string>()->default_value(""));
	options.add_options("fleet provider")(fleetProvider_,
										  R"(Provider of communication with fleet, "protobuf" or "empty")",
										  cxxopts::value<std::string>()->default_value("protobuf"));
	options.add_options("protobuf")(daemonIp_, "IPv4 address or hostname of server side application",
										  cxxopts::value<std::string>()->default_value("127.0.0.1"));
	options.add_options("protobuf")(daemonPort_, "Port of server side application",
										  cxxopts::value<int>()->default_value("1636"));
	options.add_options()(vehicleProvider_,
											R"(Choose virtual vehicle location provider, "simulation" or "gps".)",
											cxxopts::value<std::string>()->default_value("simulation"));
	options.add_options("simulation vehicle provider")(osmSpeedOverride_, "Override map speed on all points, in m/s",
													   cxxopts::value<uint32_t>());
	options.add_options("simulation vehicle provider")(osmStopWaitTime_, "Wait time in stops in seconds, default is 10s",
													   cxxopts::value<uint32_t>()->default_value("10"));
	options.add_options("gps vehicle provider")(gpsProvider_, R"(Choose gps provider, "rutx09" or "ublox".)",
										cxxopts::value<std::string>()->default_value("rutx09"));
	options.add_options("gps vehicle provider")(rutxIp_, "Modbus server address for rutx09.",
										cxxopts::value<std::string>()->default_value("127.0.0.1"));
	options.add_options("gps vehicle provider")(rutxPort_, "Modbus server port for rutx09.",
										cxxopts::value<int>()->default_value("502"));
	options.add_options("gps vehicle provider")(rutxSlaveId_, "Modbus server slave id for rutx09.",
										cxxopts::value<int>()->default_value("1"));
	options.add_options("gps vehicle provider")(stopRadius_, "Radius from stop for marking it done.",
												cxxopts::value<int>()->default_value("5"));
	options.add_options()("h, " + help_, "Print usage");

	cmdArguments_ = options.parse(argc, argv);

	if(cmdArguments_.count("help") || argc == 1) {
		std::cout << options.help() << std::endl;
	}
}

bool SettingsParser::areCmdArgumentsCorrect() {
	bool isCorrect = true;
	std::vector<std::string> requiredParams {
			osmMap_,
	};
	std::vector<std::string> allParameters = { verbose_,
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

	settings_->verbose = cmdArguments_.count(verbose_) == 1;
	settings_->mapFilePath = cmdArguments_[osmMap_].as<std::string>();
	settings_->routeName = cmdArguments_[osmRoute_].as<std::string>();
	settings_->daemonIpAddress = cmdArguments_[daemonIp_].as<std::string>();
	settings_->daemonPort = cmdArguments_[daemonPort_].as<int>();
	settings_->logPath = cmdArguments_[logPath_].as<std::string>();
	settings_->stopWaitTime = cmdArguments_[osmStopWaitTime_].as<uint32_t>();
	settings_->messagePeriodMs = cmdArguments_[statusMessagePeriod_].as<uint32_t>();
	settings_->fleetProvider = common_utils::EnumUtils::stringToFleetProvider(
			cmdArguments_[fleetProvider_].as<std::string>());
	if(cmdArguments_.count(osmSpeedOverride_)) {
		settings_->speedOverride = true;
		settings_->speedOverrideMS = cmdArguments_[osmSpeedOverride_].as<uint32_t>();
	}
	settings_->vehicleProvider = common_utils::EnumUtils::stringToVehicleProvider(
			cmdArguments_[vehicleProvider_].as<std::string>());
	settings_->gpsProvider = common_utils::EnumUtils::stringToGpsProvider(
			cmdArguments_[gpsProvider_].as<std::string>());
	settings_->rutxIp = cmdArguments_[rutxIp_].as<std::string>();
	settings_->rutxPort = cmdArguments_[rutxPort_].as<int>();
	settings_->rutxSlaveId = cmdArguments_[rutxSlaveId_].as<int>();
	settings_->stopRadius = cmdArguments_[stopRadius_].as<int>();
}
}

