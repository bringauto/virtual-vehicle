#pragma once

#include <bringauto/osm/Point.hpp>
#include <bringauto/osm/OsmObject.hpp>

#include <memory>



namespace bringauto::osm {
/**
 * Class for representing parts of routes - ways
 */
class Way: public OsmObject {
public:
	explicit Way(osmiumObjectId nodeId)
			: OsmObject(nodeId) {
	}

	/**
	 * Appending point to the end of way
	 * @param point point to be appended
	 */
	void appendPoint(const std::shared_ptr<Point> &point);

	/**
	 * Append stop point to the vector of stops
	 * @param point point to be appended
	 */
	void appendStop(const std::shared_ptr<Point> &point);

	/**
	 * Get vector of all stops on way
	 * @return vector of stops
	 */
	[[nodiscard]] const std::vector<std::shared_ptr<Point>> &getStops() const;

	/**
	 * Get vector of all points on way
	 * @return vector of points
	 */
	[[nodiscard]] const std::vector<std::shared_ptr<Point>> &getPoints() const;

private:
	/// Vector of all stops on way
	std::vector<std::shared_ptr<Point>> stops_ {};
	/// Vector of all points on way
	std::vector<std::shared_ptr<Point>> points_ {};

};
}

