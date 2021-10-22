#include <bringauto/communication/ICommunication.hpp>

void bringauto::communication::ICommunication::startWorker() {
	initializeConnection();
	workerThread_ = std::make_unique<std::thread>([this]() { periodicStatusSender(); });
}

void bringauto::communication::ICommunication::periodicStatusSender() {
	while (!globalContext_->ioContext.stopped()) {
		sendStatus();
		std::this_thread::sleep_for(std::chrono::milliseconds(statusPeriodMs_));
	}
}
