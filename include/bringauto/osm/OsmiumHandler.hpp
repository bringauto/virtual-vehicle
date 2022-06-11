#pragma once

#include <bringauto/osm/Way.hpp>
#include <bringauto/osm/Route.hpp>
#include <bringauto/osm/Point.hpp>

#include <memory>



namespace bringauto::osm {

/**
 * Handler that libosmium uses when reading file, node(), way() and relation() methods are called by osmium when corresponding objects are parsed,
 * class will parse those objects and store them
 */
class OsmiumHandler: public osmium::handler::Handler {
public:
	/**
	 * Osmium handler method for getting Way object from file, method will create new Route and fill it with Points that are included in Way,
	 * route name will not be set, route name is set in relation object
	 *
	 */
	void way(const osmium::Way &way);

	/**
	 * Osmium handler method for getting Node object from file, method will create Point instance of node with
	 * its position, stop and speed if present and appends it into points vector
	 *
	 */
	void node(const osmium::Node &node);

	/**
	 * Osmium handler method for getting Relation object from file, method will read route name from relation and add it to corresponding route
	 *
	 */
	void relation(const osmium::Relation &relation);

	/**
	 * Get all points read from .osm file
	 * @return vector of points
	 */
	[[nodiscard]] const std::vector<std::shared_ptr<Point>> &getPoints() const;

	/**
	 * Get all routes read from .osm file
	 * @return vector of routes
	 */
	[[nodiscard]] const std::vector<std::shared_ptr<Route>> &getRoutes() const;

private:
	std::vector<std::shared_ptr<Point>> points_;
	std::vector<std::shared_ptr<Way>> ways_;
	std::vector<std::shared_ptr<Route>> routes_;
};
}