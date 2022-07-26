#pragma once

#include "IVirtualVehicle.hpp"

#include <utility>



namespace bringauto::virtual_vehicle::vehicle_provider {
/**
 * @brief Class that provides vehicle location based on driving simulation on .osm map
 */
class SimVehicle: public IVirtualVehicle {
public:
	SimVehicle(const std::shared_ptr<osm::Route>& shortRoute,const std::shared_ptr<osm::Route>& longRoute, const std::shared_ptr<communication::ICommunication>& com,
			   const std::shared_ptr<GlobalContext>& globalContext): IVirtualVehicle(shortRoute, longRoute, com, globalContext){};
	/**
	 * Prepare vehicle and route for drive simulation
	 */
	void initialize() override;

private:
	std::shared_ptr<osm::Point> actualPosition_;
	std::shared_ptr<osm::Point> nextPosition_;
	double actualSpeed_ { 0 }; //meters per second
	std::vector<std::string> mission_;
	bool missionValidity_ { true };
	std::string nextStopName_;
	bool changeRoute_{false};

	long long int driveMillisecondLeft_ { 0 };
	uint64_t inStopMillisecondsLeft_ { 0 };

	communication::Status::State state_ { communication::Status::State::IDLE };

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
	void updateVehicleState(communication::Status::State state);

	/**
	 * check if current position is in next stop
	 */
	int checkForStop();
};
}

