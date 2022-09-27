#pragma once

#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>
#include <bringauto/osm/Route.hpp>

namespace bringauto::virtual_vehicle::gps_provider{
class MapGps: public IGpsProvider {
public:
	MapGps(const std::shared_ptr<osm::Route>& route);

	GpsPosition getPosition() override;

	float getSpeed() override;
private:
	std::shared_ptr<osm::Route> route_;
};
}
