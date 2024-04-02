#pragma once

#include <bringauto/virtual_vehicle/vehicle_provider/IVirtualVehicle.hpp>

#include <utility>



namespace bringauto::virtual_vehicle::vehicle_provider {
/**
 * @brief Class that provides vehicle location based on driving simulation on .osm map
 */
class SimVehicle: public IVirtualVehicle {
public:
	SimVehicle(const std::shared_ptr<communication::ICommunication> &com,
			   const std::shared_ptr<GlobalContext> &globalContext): IVirtualVehicle(com, globalContext) {};

	/**
	 * Prepare vehicle and route for drive simulation
	 */
	void initialize() override;

private:
	std::shared_ptr<osm::Point> actualPosition_;
	std::shared_ptr<osm::Point> nextPosition_;
	double actualSpeed_ { 0 }; //meters per second
	std::vector<osm::Route::Station> mission_;
	bool missionValidity_ { true };
	osm::Route::Station nextStop_;
	bool changeRoute_ { false };
	bool checkStations_ { false };
	std::string actualRouteName_ {};
	std::string nextRouteName_ {};

	long long int driveMillisecondLeft_ { 0 };
	uint64_t inStopMillisecondsLeft_ { 0 };

	communication::EAutonomyState state_ { communication::EAutonomyState::E_IDLE };

	void changeRoute();

	void nextEvent() override;

	void handleIdleEvent();

	void handleDriveEvent();

	void handleInStopEvent();

	void handleObstacleEvent();

	void handleErrorEvent();

	/**
	 * Vehicle will simulate driving from current point to next point
	 */
	void setNextPosition();

	/**
	 * push updated status to com_
	 */
	void request();

	/**
	 * decide what action will be happening after new command is received
	 */
	void evaluateCommand();

	/**
	 * Set vehicle state from argument, set 0 speed if state is in_stop
	 * @param state state to be set
	 */
	void updateVehicleState(communication::EAutonomyState state);

	/**
	 * check if current position is in next stop
	 * @return true if vehicle is in stop
	 */
	bool checkForStop();


};
}

