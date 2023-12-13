#include <bringauto/common_utils/CommonUtils.hpp>

#include <osmium/geom/haversine.hpp>

#include <sstream>
#include <iomanip>



namespace bringauto::common_utils {

std::string CommonUtils::constructMissionString(const std::vector<osm::Route::Station> &mission) {
	std::ostringstream missionString;
	if(mission.empty()) {
		return "[]";
	}
	missionString << "[";
	for(const auto &stop: mission) {
		missionString << "\"" << stop.name << "\",";
	}

	/// Remove last comma
	auto len = missionString.tellp();
	if(len > 0) {
		missionString.seekp(len - std::streamoff(1));
	}

	missionString << "]";
	return missionString.str();
}

double
CommonUtils::calculateDistanceInMeters(const osm::Point &a, const osm::Point &b) {
	return osmium::geom::haversine::distance(
			osmium::geom::Coordinates { a.getLatitude(), a.getLongitude() },
			osmium::geom::Coordinates { b.getLatitude(), b.getLongitude() });
}

double
CommonUtils::calculateDistanceInMeters(double aLatitude, double aLongitude, double bLatitude, double bLongitude) {
	return osmium::geom::haversine::distance(
			osmium::geom::Coordinates { aLatitude, aLongitude },
			osmium::geom::Coordinates { bLatitude, bLongitude });
}

uint64_t CommonUtils::timeToDriveInMilliseconds(double distanceInMeters, double speedMeterPerS) {
	if(std::fabs(speedMeterPerS) < std::numeric_limits<double>::epsilon()){
		return 0;
	}
	return uint64_t((distanceInMeters/speedMeterPerS)*1000);
}

std::string CommonUtils::exportRouteToFleetInitFormat(const std::shared_ptr<osm::Route> &route) {
	if(!route) {
		return "";
	}
	std::string exportedRoute;
	route->prepareRoute();
	auto startPoint = route->getPosition();
	route->setNextPosition();
	while(route->getPosition() != startPoint) {
		exportedRoute += exportJsonPosition(*route->getPosition()) + ",";
		route->setNextPosition();
	}
	return exportedRoute;
}

std::string CommonUtils::exportJsonPosition(const osm::Point &position) {
	std::stringstream ss;
	ss << "{ \"latitude\":";
	ss << std::fixed << std::setprecision(7) << position.getLatitude();
	ss << ", \"longitude\":";
	ss << std::fixed << std::setprecision(7) << position.getLongitude();
	ss << ", \"stationName\": " << (position.getName().empty() ? "null" : ("\"" + position.getName() + "\"")) << "}";
	return ss.str();
}

bool CommonUtils::compareMissions(const std::vector<osm::Route::Station> &mission1,
								  const std::vector<osm::Route::Station> &mission2) {
	if(mission1.size() != mission2.size()) {
		return false;
	}

	for(size_t i = 0; i < mission1.size(); i++) {
		if(mission1[i].name != mission2[i].name) {
			return false;
		}
		// Use precision to compare equality of 'double' values
		if(std::abs(mission1[i].latitude - mission2[i].latitude) > PRECISION_) {
			return false;
		}
		if(std::abs(mission1[i].longitude - mission2[i].longitude) > PRECISION_) {
			return false;
		}
	}
	return true;
}


}
