#include <bringauto/communication/ICommunication.hpp>



namespace bringauto::communication {
Command bringauto::communication::ICommunication::getCommand() {
	return currentCommand_;
}

bool ICommunication::isConnected() const {
	return isConnected_;
}
}


