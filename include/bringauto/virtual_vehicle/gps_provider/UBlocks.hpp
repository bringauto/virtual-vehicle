#pragma once

#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>

namespace bringauto::virtual_vehicle::gps_provider {
/**
 * @brief Class for working with UBlocks gps tracker
 * this class is not implemented yet
 */
class UBlocks: public IGpsProvider {
public:
	GpsPosition getPosition() override;

	float getSpeed() override;
};
}