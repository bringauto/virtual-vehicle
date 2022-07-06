
#include <bringauto/osm/Point.hpp>


namespace bringauto::osm{
double Point::getLongitude() const {
	return longitude_;
}

double Point::getLatitude() const {
	return latitude_;
}

const double &Point::getSpeedInMetersPerSecond() const {
	return speedInMetersPerSecond_;
}

bool Point::isStop() const {
	return stop_;
}

void Point::setSpeed(const double &speed) {
	speedInMetersPerSecond_ = speed;
}

const std::string &Point::getName() const {
	return name_;
}
}