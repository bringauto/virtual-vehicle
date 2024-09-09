#pragma once

#include <bringauto/virtual_vehicle/vehicle_provider/IVirtualVehicle.hpp>
#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>

#include <chrono>


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
	static constexpr double MAX_STOP_SPEED = 0.5;
	static constexpr std::chrono::seconds TIMER_DURATION = std::chrono::seconds(5);

//	struct Timer {
	std::chrono::time_point<std::chrono::steady_clock> timerStart_;
	bool timerRunning_ = false;
//	} timer_;

	void nextEvent() override;

	void updatePosition();

	void makeRequest();

	void evaluateCommand();


};
}