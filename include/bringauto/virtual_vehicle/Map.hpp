#pragma once

#include <bringauto/osm/OsmiumHandler.hpp>
#include <string>



namespace bringauto::virtual_vehicle {
/**
 * @brief Class for processing .osm map files, that are used for simulated driving
 */
class Map {
public:
	/**
	 * Load points and routes from .osm file using libosmium
	 *
	 * @param filePath full path to .osm file
	 */
	void loadMapFromFile(const std::string &filePath);

	/**
	 * Get specific route from map
	 *
	 * @param routeName name of one of routes specified in .osm file
	 */
	std::shared_ptr<osm::Route> getRoute(const std::string &routeName);

    /**
     * calls prepareRoute() on all routes in vector routes_
     */
    void prepareRoutes();

	/**
	 * @brief Overrides speed on all routes to speed given by param
	 * @param speed
	 */
	void speedOverride(unsigned int speed);

    /**
     * @return private vector routes_
     */
    std::vector<std::shared_ptr<osm::Route>> getAllRoutes() {return routes_;};

private:
	std::vector<std::shared_ptr<osm::Route>> routes_;
	std::vector<std::shared_ptr<osm::Point>> points_;
};
}
