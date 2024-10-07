#include <bringauto/virtual_vehicle/gps_provider/UBlocks.hpp>
#include <bringauto/settings/LoggerId.hpp>

namespace bringauto::virtual_vehicle::gps_provider{
float UBlocks::getSpeed() {
	settings::Logger::logError("UBlocks gps provider is not implemented!");
	return 0;
}

GpsPosition UBlocks::getPosition() {
	settings::Logger::logError("UBlocks gps provider is not implemented!");
	return {};
}
}