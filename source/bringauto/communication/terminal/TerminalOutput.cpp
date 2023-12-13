#include <bringauto/communication/terminal/TerminalOutput.hpp>

#include <bringauto/logging/Logger.hpp>


namespace bringauto::communication::terminal {
bool TerminalOutput::initializeConnection() {
	bringauto::logging::Logger::logInfo("Empty connection established");
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::connectedState);
#endif
	return true;
}

bool TerminalOutput::makeRequest(const Status &status) {
	bringauto::logging::Logger::logInfo("Moved to position lat: {} lon: {} speed: {}", status.getLatitude(),
										status.getLongitude(), status.getSpeed());

	bringauto::logging::Logger::logInfo("Sending status");
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::statusSentState);
#endif
	bringauto::logging::Logger::logInfo("Received command");
#ifdef STATE_SMURF
	globalContext_->transitions->goToState(bringauto::settings::commandReceivedState);
#endif
	return true;
}
}



