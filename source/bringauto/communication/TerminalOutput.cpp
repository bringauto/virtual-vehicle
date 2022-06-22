#include <bringauto/communication/TerminalOutput.hpp>

#include <bringauto/logging/Logger.hpp>

#include <string>



void bringauto::communication::TerminalOutput::initializeConnection() {
	bringauto::logging::Logger::logInfo("Empty connection established");
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::connectedState);
#endif
}

void bringauto::communication::TerminalOutput::makeRequest(const Status &status) {
	bringauto::logging::Logger::logInfo("Moved to position lat: {} lon: {} speed: {}", status.latitude,
										status.longitude, status.speed);
	sendStatus(status);
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::statusSentState);
#endif
	receiveCommand();
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::commandReceivedState);
#endif
}

bringauto::communication::TerminalOutput::~TerminalOutput() {
}

void bringauto::communication::TerminalOutput::sendStatus(const Status &status) {
	bringauto::logging::Logger::logInfo("Sending status");
}

void bringauto::communication::TerminalOutput::receiveCommand() {
	bringauto::logging::Logger::logInfo("Received command");
}


