
#include <bringauto/osm/Point.hpp>


double bringauto::osm::Point::getLongitude() const {
	return longitude_;
}

double bringauto::osm::Point::getLatitude() const {
	return latitude_;
}

const double &bringauto::osm::Point::getSpeedInMetersPerSecond() const {
	return speedInMetersPerSecond_;
}

bool bringauto::osm::Point::isStop() const {
	return stop_;
}

void bringauto::osm::Point::setSpeed(const double &speed) {
	speedInMetersPerSecond_ = speed;
}

const std::string &bringauto::osm::Point::getName() const {
	return name_;
}