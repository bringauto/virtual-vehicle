#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>



namespace bringauto::communication {
Command bringauto::communication::ICommunication::getCommand() {
	return command_;
}

std::ostream &operator<<(std::ostream &stream, const Command &command) {
	stream << "action: " << common_utils::EnumUtils::enumToString(command.action) << ", route: " << command.route
		   << " mission stops: ";
	for(const auto &stop: command.stops) {
		stream << stop << ", ";
	}
	stream << "route stations: ";
	for(const auto &station: command.routeStations) {
		stream << station.name << ", ";
	}
	return stream;
}

std::ostream &operator<<(std::ostream &stream, const Status &status) {
	stream << "state: " << common_utils::EnumUtils::enumToString(status.state) << " latitude: "
		   << status.latitude << " longitude: " << status.longitude << " next stop: " << status.nextStop;
	return stream;
}
}


