#include <bringauto/common_utils/CommonUtils.hpp>
#include <osmium/geom/haversine.hpp>



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

uint64_t CommonUtils::timeToDriveInMilliseconds(double distanceInMeters, double speedInMetersPerSecond) {
	return uint64_t((distanceInMeters/speedInMetersPerSecond)*1000);
}

}
