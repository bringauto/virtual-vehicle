#include <bringauto/communication/TerminalOutput.hpp>
#include <bringauto/logging/Logger.hpp>

#include <iostream>
#include <string>

void bringauto::communication::TerminalOutput::initializeConnection() {
    bringauto::logging::Logger::logInfo("Dummy connection established");
}

void bringauto::communication::TerminalOutput::sendStatus(double lat, double lon, double speed, State state, const std::string& nextStop) {
    std::stringstream ss;
    ss << "Moved to position lat: " << lat << " lon: " << lon << " speed: " << speed;
    bringauto::logging::Logger::logInfo(ss.str());
}

bringauto::communication::TerminalOutput::~TerminalOutput() {
    bringauto::logging::Logger::logInfo("Overrided terminal destructor");
}


