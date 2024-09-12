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

MapGps::MapGps(const std::string &mapName, const std::string &routeName) {
	map_.loadMapFromFile(mapName);
	map_.prepareRoutes();
	if(!routeName.empty()) {
		route_ = map_.getRoute(routeName);
	} else {
		route_ = map_.getAllRoutes()[0];
	}
}
}
