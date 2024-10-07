#pragma once

#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>
#include <bringauto/osm/Route.hpp>
#include <bringauto/virtual_vehicle/Map.hpp>



namespace bringauto::virtual_vehicle::gps_provider {
/**
 * @brief Class for testing purposes, that acts as GPS vehicle, but the location is provided based on a map
 *
 * Map vehicle cannot change route and is always driving on the route from config.
 */
class MapGps: public IGpsProvider {
public:
	MapGps(const std::string &mapName, const std::string &routeName);

	/**
	 * @brief Get gps position from map
	 * @return float latitude
	 */
	GpsPosition getPosition() override;

	/**
	 * @brief Get current speed from map
	 * @return float speed
	 */
	float getSpeed() override;

private:
	bringauto::virtual_vehicle::Map map_;
	std::shared_ptr<osm::Route> route_;
};
}
