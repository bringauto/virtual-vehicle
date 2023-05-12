#pragma once

#include <bringauto/osm/OsmiumHandler.hpp>

#include <string>
#include <vector>



namespace bringauto::common_utils {
/**
 * @brief Class collecting common standalone functionality used through the project
 */
class CommonUtils {
public:
	CommonUtils() = delete;
	/**
	 * @brief Return list of stops in mission as one string in format [stop1, stop2 ...]
	 * @return formatted list of stops in string
	 */
	static std::string constructMissionString(const std::vector<std::string> &mission);

	/**
	 * @brief Method will calculate and return distance between point a and point b
	 * @return distance between points in meters
	 */
	static double calculateDistanceInMeters(const std::shared_ptr<osm::Point> &a, const std::shared_ptr<osm::Point> &b);

	/**
	 * @brief Method will calculate and return distance between point a and point b
	 * @return distance between points in meters
	 */
	static double calculateDistanceInMeters(double aLatitude, double aLongitude, double bLatitude, double bLongitude);

	/**
	 * @brief Calculates time in milliseconds that will take to drive given distance while moving at given speed
	 * @param distanceInMeters distance that need to be traveled in meters
	 * @param speedInMetersPerSecond speed in meters per second
	 * @return number of milliseconds that will take to travel the distance at the speed
	 */
	static uint64_t timeToDriveInMilliseconds(double distanceInMeters, double speedInMetersPerSecond);

	/**
	 * @brief Create a string with all points in given route in fleet-init utility format
	 * @param route route to be printed
	 *
	 * @return string containing list of points in json format that can be used in fleet init config
	 */
	static std::string exportRouteToFleetInitFormat(const std::shared_ptr<osm::Route>& route);

private:
	static std::string exportJsonPosition(const std::shared_ptr<osm::Point>& position);
};
}