#include <bringauto/virtual_vehicle/gps_provider/UBlocks.hpp>
#include <bringauto/logging/Logger.hpp>

namespace bringauto::virtual_vehicle::gps_provider{
float UBlocks::getLatitude() {
	logging::Logger::logError("UBlocks gps provider is not implemented!");
	return 0;
}

float UBlocks::getLongitude() {
	logging::Logger::logError("UBlocks gps provider is not implemented!");
	return 0;
}

float UBlocks::getAltitude() {
	logging::Logger::logError("UBlocks gps provider is not implemented!");
	return 0;
}

float UBlocks::getSpeed() {
	logging::Logger::logError("UBlocks gps provider is not implemented!");
	return 0;
}
}