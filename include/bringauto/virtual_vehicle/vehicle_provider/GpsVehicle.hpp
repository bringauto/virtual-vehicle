#pragma once

#include <bringauto/virtual_vehicle/vehicle_provider/IVirtualVehicle.hpp>
#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>



namespace bringauto::virtual_vehicle::vehicle_provider {
/**
 * @brief Class that provides vehicle location based on real gps tracker
 */
class GpsVehicle final: public IVirtualVehicle {
public:
	GpsVehicle(const std::shared_ptr<communication::ICommunication> &com,
			   const std::shared_ptr<GlobalContext> &globalContext): IVirtualVehicle(com, globalContext) {};

	/**
	 * @brief Initialize connection to GPS tracker selected by cmd argument
	 */
	void initialize() override;


private:
	std::unique_ptr<gps_provider::IGpsProvider> gpsProvider_;
	communication::Status status_;
	double eventDelayInSec_;
	std::vector<std::shared_ptr<osm::Point>> stops_;
	std::shared_ptr<osm::Point> currentStop_;

	void nextEvent() override;

	void updatePosition();

	void makeRequest();

	void evaluateCommand();

};
}