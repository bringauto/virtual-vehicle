#include <bringauto/communication/Command.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>



namespace bringauto::communication {
std::ostream &operator<<(std::ostream &stream, const Command &command) {
	stream << "action: " << common_utils::EnumUtils::enumToString(command.getAction()) << ", route: " << command.getRoute()
		   << " mission stops: ";
	for(const auto &stop: command.getMission()) {
		stream << "[" << stop.name << ";" << stop.latitude << ";" << stop.longitude << "],";
	}
	return stream;
}

const std::vector<osm::Route::Station> &Command::getMission() const {
	return mission_;
}

void Command::setMission(const std::vector<osm::Route::Station> &mission) {
	mission_ = mission;
}

EAutonomyAction Command::getAction() const {
	return action_;
}

void Command::setAction(EAutonomyAction action) {
	action_ = action;
}

const std::string &Command::getRoute() const {
	return route_;
}

void Command::setRoute(const std::string &route) {
	route_ = route;
}
}