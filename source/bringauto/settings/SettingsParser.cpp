#include <bringauto/settings/SettingsParser.hpp>



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
	cxxopts::Options options { "BringAuto virtual vehicle utility", "BringAuto virtual vehicle utility" };
	options.add_options()
				   ("v, verbose", "Verbose into terminal and log info messages");
	options.add_options()("m, map", "Path to .osm map file", cxxopts::value<std::string>());
	options.add_options()("r, route", "Name of route that will be used for vehicle", cxxopts::value<std::string>());
	options.add_options()("l, log-path", "Path to logs", cxxopts::value<std::string>()->default_value("./"));
	options.add_options()("i, ip", "IPv4 address or hostname of server side application",
						  cxxopts::value<std::string>());
	options.add_options()("p, port", "Port of server side application", cxxopts::value<int>());
	options.add_options()("w, wait", "Wait time in stops in seconds, default is 10s",
						  cxxopts::value<uint32_t>()->default_value("10"));
	options.add_options()("period", "Period in ms for sending status messages",
						  cxxopts::value<uint32_t>()->default_value("1000"));

	options.add_options()("speed-override", "Override map speed on all points, in m/s",
						  cxxopts::value<uint32_t>());
	options.add_options()("h, help", "Print usage");

	cmdArguments_ = options.parse(argc, argv);

	if(cmdArguments_.count("help") || argc == 1) {
		std::cout << options.help() << std::endl;
	}
}

bool SettingsParser::areCmdArgumentsCorrect() {
	bool isCorrect = true;
	std::vector<std::string> requiredParams {
			"map",
			"route",
			"ip",
			"port",
	};
	std::vector<std::string> allParameters = { "verbose",
											   "map",
											   "route",
											   "log-path",
											   "ip",
											   "port",
											   "wait",
											   "period",
											   "speed-override",
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

	if(!std::filesystem::exists(settings_->mapFilePath)) {
		std::cerr << "Given map path (" << settings_->mapFilePath << ") does not exist." << std::endl;
		isCorrect = false;
	}

	if(!std::filesystem::exists(settings_->logPath)) {
		std::cerr << "Given log path (" << settings_->logPath << ") does not exist." << std::endl;
		isCorrect = false;
	}

	return isCorrect;
}

std::shared_ptr<Settings> SettingsParser::getSettings() {
	return settings_;
}

void SettingsParser::fillSettings() {
	settings_ = std::make_shared<Settings>();

	settings_->verbose = cmdArguments_.count("verbose") == 1;
	settings_->mapFilePath = cmdArguments_["map"].as<std::string>();
	settings_->routeName = cmdArguments_["route"].as<std::string>();
	settings_->ipAddress = cmdArguments_["ip"].as<std::string>();
	settings_->port = cmdArguments_["port"].as<int>();
	settings_->logPath = cmdArguments_["log-path"].as<std::string>();
	settings_->stopWaitTime = cmdArguments_["wait"].as<uint32_t>();
	settings_->messagePeriodMs = cmdArguments_["period"].as<uint32_t>();
	if(cmdArguments_.count("speed-override")) {
		settings_->speedOverride = true;
		settings_->speedOverrideMS = cmdArguments_["speed-override"].as<uint32_t>();
	}
}
}

