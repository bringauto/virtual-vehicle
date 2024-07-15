#pragma once

#include <bringauto/osm/Way.hpp>
#include <bringauto/osm/Point.hpp>

#include <osmium/geom/haversine.hpp>

#include <vector>
#include <optional>



namespace bringauto::osm {
/**
 * Class for representing both osm Way and Relation
 */
class Route: public OsmObject {
public:
	explicit Route(osmiumObjectId nodeId, const std::string &routeName)
			: OsmObject(nodeId), routeName_(routeName) {
	}

	explicit Route(osmiumObjectId nodeId)
			: OsmObject(nodeId) {
	}

	struct Station {
		std::string name;
		double latitude;
		double longitude;
	};

	/**
	 * Only some points have speed tag, this method will propagate speed into rest of points
	 */
	void propagateSpeed();

	/**
	 * Function that will prepare route, it will set actual position to starting (first) point and determine if route is routeIsCircular_ or not
	 */
	void prepareRoute();

	/**
	 * Set route name
	 * @param routeName name of route
	 */
	void setRouteName(const std::optional<std::string> &routeName);

	/**
	 * Get name of route
	 * @return name of route
	 */
	std::string getRouteName();

	/**
	 * Get actual position on route
	 * @return point representing position
	 */
	std::shared_ptr<Point> getPosition();

	/**
	 * @brief Get vector of all stops present on the route
	 * @return vector of stops
	 */
	std::vector<std::shared_ptr<Point>> getStops();

	/**
	 * Move on point vector, if end is reached route will be connected to start if circular or reversed
	 */
	void setNextPosition();

	/**
	 * Check if all stops are present on route
	 * @param stopNames vector containing all of stop names that will be checked
	 * @return true if all stops are on route
	 */
	bool areStopsPresent(const std::vector<osm::Route::Station> &stops);

	/**
	 * @brief Get closest point to given point that is closer than POINT_TOLERANCE_M
	 * @param point point to which the closest point will be found
	 * @return closest point or nullptr if no point is found
	 */
	std::shared_ptr<Point> getClosestPoint(const Point &point) const;

	void setPositionAndDirection(const Point &actualPosition, const std::string &nextStopName);

	/**
	 * Append all points and stops from way into route
	 * @param way way to be appended to route
	 */
	void appendWay(const std::shared_ptr<Way> &way);

	/**
	 * @brief Overrides speed for each point of route to speed given by param
	 * @param speed
	 */
	void speedOverride(unsigned int speed);

	/**
	 * @brief Compare route with given command stations
	 * @param commandStations stations to compare
	 */
	void compareStations(std::vector<Station> commandStations);

private:
	/// Threshold for distance between routes
	const double ROUTES_DISTANCE_THRESHOLD_M { 50.0 };
	/// Limit distance from stop and end point to determine if route is circular or not
	const double CIRCULAR_ROUTE_THRESHOLD_M { 10.0 };
	/// Tolerance for comparing points
	static constexpr float POINT_TOLERANCE_M { 0.5 };
	static constexpr float DISTANCE_TOLERANCE_M { 0.001 };

	/// All points in route
	std::vector<std::shared_ptr<Point>> points_ {};
	/// All stops in route
	std::vector<std::shared_ptr<Point>> stops_ {};
	/// Name of route
	std::optional<std::string> routeName_;
	/// Iterator pointing to actual position on route
	std::vector<std::shared_ptr<Point>>::iterator positionIt;
	/// Flag indicating if route is circular or not
	bool routeIsCircular_ { false };

};
}

