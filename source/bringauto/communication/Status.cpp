#include <bringauto/communication/Status.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>



namespace bringauto::communication {
std::ostream &operator<<(std::ostream &stream, const Status &status) {
	stream << "state: " << common_utils::EnumUtils::enumToString(status.getState()) << " latitude: "
		   << status.getLatitude() << " longitude: " << status.getLongitude() << " next stop: "
		   << status.getNextStop().name;
	return stream;
}

double Status::getLongitude() const {
	return longitude_;
}

void Status::setLongitude(double longitude) {
	longitude_ = longitude;
}

double Status::getLatitude() const {
	return latitude_;
}

void Status::setLatitude(double latitude) {
	latitude_ = latitude;
}

double Status::getSpeed() const {
	return speed_;
}

void Status::setSpeed(double speed) {
	speed_ = speed;
}

EAutonomyState Status::getState() const {
	return state_;
}

void Status::setState(EAutonomyState state) {
	state_ = state;
}

const osm::Route::Station &Status::getNextStop() const {
	return nextStop_;
}

void Status::setNextStop(const osm::Route::Station &nextStop) {
	nextStop_ = nextStop;
}
}