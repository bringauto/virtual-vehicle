#include <bringauto/virtual_vehicle/gps_provider/UBlocks.hpp>
#include <bringauto/logging/Logger.hpp>

namespace bringauto::virtual_vehicle::gps_provider{
float UBlocks::getSpeed() {
	logging::Logger::logError("UBlocks gps provider is not implemented!");
	return 0;
}

GpsPosition UBlocks::getPosition() {
	logging::Logger::logError("UBlocks gps provider is not implemented!");
	return {};
}
}