#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>



namespace bringauto::communication {
Command bringauto::communication::ICommunication::getCommand() {
	return command_;
}

std::ostream &operator<<(std::ostream &stream, const Command &command) {
	stream << "action: " << common_utils::EnumUtils::enumToString(command.action) << " stops:";
	for(const auto &stop: command.stops) {
		stream << " " << stop;
	}
	return stream;
}

std::ostream &operator<<(std::ostream &stream, const Status &status) {
	stream << "state: " << common_utils::EnumUtils::enumToString(status.state) << " latitude: "
		   << status.latitude << " longitude: " << status.longitude << " next stop: " << status.nextStop;
	return stream;
}
}


