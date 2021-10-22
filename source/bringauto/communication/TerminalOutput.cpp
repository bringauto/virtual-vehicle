#include <bringauto/communication/TerminalOutput.hpp>

#include <bringauto/logging/Logger.hpp>

#include <string>

void bringauto::communication::TerminalOutput::initializeConnection() {
	bringauto::logging::Logger::logInfo("Dummy connection established");
}

void bringauto::communication::TerminalOutput::updateStatus(double lat, double lon, double speed, State state,
															const std::string &nextStop) {
	bringauto::logging::Logger::logInfo("Moved to position lat: {} lon: {} speed: {}", lat, lon, speed);
}

bringauto::communication::TerminalOutput::~TerminalOutput() {
	bringauto::logging::Logger::logInfo("Overrided terminal destructor");
}


