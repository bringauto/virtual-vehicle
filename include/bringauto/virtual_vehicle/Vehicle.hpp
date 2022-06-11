#pragma once

#include <bringauto/osm/OsmiumHandler.hpp>
#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <utility>



namespace bringauto::virtual_vehicle {
class Vehicle {
public:
	Vehicle(std::shared_ptr<osm::Route> route,
			std::shared_ptr<communication::ICommunication> com,
			std::shared_ptr<GlobalContext> globalContext): route_(std::move(route)),
														   com_(std::move(com)),
														   globalContext_(std::move(
																   globalContext)) {};

	/**
	 * Prepare vehicle and route for drive simulation
	 */
	void initialize();

	/**
	 * Simulate vehicle driving
	 */
	void drive();

private:
	std::shared_ptr<osm::Route> route_;
	std::shared_ptr<communication::ICommunication> com_;
	std::shared_ptr<GlobalContext> globalContext_;

	std::shared_ptr<osm::Point> actualPosition_;
	std::shared_ptr<osm::Point> nextPosition_;
	double actualSpeed_ { 0 }; //meters per second
	std::vector<std::string> mission_;
	bool missionValidity_ { true };
	std::string nextStopName_;

	long long int driveMillisecondLeft_ { 0 };
	uint64_t inStopMillisecondsLeft_ { 0 };

	communication::Status::State state_ { communication::Status::State::IDLE };

	void nextEvent();

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

