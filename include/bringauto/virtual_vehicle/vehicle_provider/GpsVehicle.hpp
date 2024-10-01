#pragma once

#include <bringauto/virtual_vehicle/vehicle_provider/IVirtualVehicle.hpp>
#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>
#include <bringauto/common_utils/Timer.hpp>


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
	void checkForStop();
	std::unique_ptr<gps_provider::IGpsProvider> gpsProvider_;
	communication::Status status_;
	double eventDelayInSec_;
	std::shared_ptr<osm::Route::Station> nextStop_;

	/// The car speed can be > 0 even if the car is stopped, because of the GPS inaccuracy
	/// static constexpr double MAX_STOP_SPEED = 0.5;

	common_utils::Timer timer_;

	void nextEvent() override;

	void updatePosition();

	void makeRequest();

	void evaluateCommand();


};
}