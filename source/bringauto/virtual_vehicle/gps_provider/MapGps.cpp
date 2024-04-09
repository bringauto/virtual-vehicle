#include <bringauto/virtual_vehicle/gps_provider/MapGps.hpp>

namespace bringauto::virtual_vehicle::gps_provider{

GpsPosition MapGps::getPosition() {
	GpsPosition position{};
	auto routePosition = route_->getPosition();
	route_->setNextPosition();
	position.latitude = routePosition->getLatitude();
	position.longitude = routePosition->getLongitude();
	return position;
}

float MapGps::getSpeed() {
	return 0;
}

MapGps::MapGps(const std::shared_ptr<osm::Route> &route): route_(route) {
	route_->prepareRoute();
}
}
