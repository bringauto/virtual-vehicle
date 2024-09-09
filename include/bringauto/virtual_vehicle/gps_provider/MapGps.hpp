#pragma once

#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>
#include <bringauto/osm/Route.hpp>



namespace bringauto::virtual_vehicle::gps_provider {
/**
 * @brief Class for testing purposes, that acts as GPS vehicle, but the location is provided based on a map
 */
class MapGps: public IGpsProvider {
public:
	MapGps(const std::shared_ptr<osm::Route> &route);

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
	std::shared_ptr<osm::Route> route_;
};
}
