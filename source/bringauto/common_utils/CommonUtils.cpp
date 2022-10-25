#include <bringauto/common_utils/CommonUtils.hpp>

#include <osmium/geom/haversine.hpp>

#include <sstream>
#include <iomanip>



namespace bringauto::common_utils {

std::string CommonUtils::constructMissionString(const std::vector<std::string> &mission) {
	std::string missionString { "[" };
	for(const auto &stopName: mission) {
		missionString += "\"" + stopName + "\", ";
	}
	missionString += "]";
	return missionString;
}

double
CommonUtils::calculateDistanceInMeters(const std::shared_ptr<osm::Point> &a, const std::shared_ptr<osm::Point> &b) {
	return osmium::geom::haversine::distance(
			osmium::geom::Coordinates { a->getLatitude(), a->getLongitude() },
			osmium::geom::Coordinates { b->getLatitude(), b->getLongitude() });
}

double
CommonUtils::calculateDistanceInMeters(double aLatitude, double aLongitude, double bLatitude, double bLongitude) {
	return osmium::geom::haversine::distance(
			osmium::geom::Coordinates { aLatitude, aLongitude },
			osmium::geom::Coordinates { bLatitude, bLongitude });
}

uint64_t CommonUtils::timeToDriveInMilliseconds(double distanceInMeters, double speedInMetersPerSecond) {
	return uint64_t((distanceInMeters/speedInMetersPerSecond)*1000);
}

std::string CommonUtils::exportRouteToFleetInitFormat(const std::shared_ptr<osm::Route> &route) {
	std::string exportedRoute;
	route->prepareRoute();
	auto startPoint = route->getPosition();
	route->setNextPosition();
	while(route->getPosition() != startPoint) {
		exportedRoute += exportJsonPosition(route->getPosition());
		route->setNextPosition();
	}
	return exportedRoute;
}

std::string CommonUtils::exportJsonPosition(const std::shared_ptr<osm::Point> &position) {
	std::stringstream ss;
	ss << "{ \"latitude\":";
	ss << std::fixed << std::setprecision(7) << position->getLatitude();
	ss << ", \"longitude\":";
	ss << std::fixed << std::setprecision(7) << position->getLongitude();
	ss << ", \"stationName\": "<< (position->getName().empty()? "null": ("\"" + position->getName() + "\"")) << "},";
	return ss.str();
}


}
