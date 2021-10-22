#pragma once

#include <bringauto/osm/OsmiumHandler.hpp>
#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <utility>

namespace bringauto::virtual_vehicle {
	class Vehicle {
	public:
		Vehicle(std::shared_ptr<bringauto::osm::Route> route,
				std::shared_ptr<bringauto::communication::ICommunication> com,
				std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> globalContext) : route_(std::move(route)),
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
		std::shared_ptr<bringauto::osm::Route> route_;
		std::shared_ptr<bringauto::communication::ICommunication> com_;
		std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> globalContext_;

		std::shared_ptr<bringauto::osm::Point> actualPosition_;
		std::shared_ptr<bringauto::osm::Point> nextPosition_;
		double actualSpeed_{0}; //m/s
		std::vector<std::string> mission_;
		bool missionValidity_{true};
		std::string nextStopName_;

		bringauto::communication::ICommunication::State state_{bringauto::communication::ICommunication::State::IDLE};

		/**
		 * Vehicle will simulate driving from current point to next point
		 */
		void driveToNextPosition();

		/**
		 * Method will calculate and return distance between actual point and next point
		 * @return distance to next point on route in meters
		 */
		double distanceToNextPosition();

		/**
		 * Method will wait for stopTimeSec_ if actualPosition_ is a stop
		 */
		void waitIfStopOrIdle();

		/**
		 * push updated status to com_
		 */
		void updateVehicleStatus();

		/**
		 * decide what action will be happening after new command is received
		 */
		void evaluateCommand();

		/**
		 * Check if actual stops are different from stops given by argument
		 * @param stopNames top list for comparison
		 * @return true if stops have been changed
		 */
		bool isChangeInMission(const std::vector<std::string> &stopNames);

		/**
		 * set next stop from stoplist
		 */
		void setNextStop();

		/**
		 * Set vehicle state from argument, set 0 speed if state is in_stop
		 * @param state state to be set
		 */
		void updateVehicleState(bringauto::communication::ICommunication::State state);

		/**
		 * check if current position is in next stop
		 */
		void checkForStop();

		/**
		 * set vehicle state from received action
		 * @param action
		 */
		void setVehicleStateFromCommand(bringauto::communication::ICommunication::Command::Action action);

		/**
		 * update list of stops if changed
		 * @param mission
		 */
		void updateMissionFromCommand(const std::vector<std::string> &mission);

		/**
		 * return list of stops in mission as one string
		 * @return formated list of stops in string
		 */
		std::string constructMissionString();
	};
}

