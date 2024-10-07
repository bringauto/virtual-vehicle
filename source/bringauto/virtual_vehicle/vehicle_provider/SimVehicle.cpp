#include <bringauto/virtual_vehicle/vehicle_provider/SimVehicle.hpp>
#include <bringauto/common_utils/CommonUtils.hpp>

#include <bringauto/settings/LoggerId.hpp>

#include <thread>
#include <bringauto/common_utils/EnumUtils.hpp>

namespace bringauto::virtual_vehicle::vehicle_provider {
void SimVehicle::initialize() {
	map_.loadMapFromFile(globalContext_->settings->mapFilePath);
	if(globalContext_->settings->speedOverride) {
		map_.speedOverride(globalContext_->settings->speedOverrideMS);
	}
	map_.prepareRoutes();

	if(!globalContext_->settings->routeName.empty()) { /// Override default route by argument
		actualRouteName_ = globalContext_->settings->routeName;
		actualRoute_ = map_.getRoute(actualRouteName_);
	} else { /// Make actualRoute_ the first in the vector for initialization reason
		actualRoute_ = map_.getAllRoutes()[0];
		actualRouteName_ = actualRoute_->getRouteName();
	}
	updateVehicleState(communication::EAutonomyState::E_IDLE);
	com_->initializeConnection();

	setNextPosition();
}

void SimVehicle::nextEvent() {
	switch(state_) {
		case communication::EAutonomyState::E_IDLE:
			handleIdleEvent();
			break;
		case communication::EAutonomyState::E_DRIVE:
			handleDriveEvent();
			break;
		case communication::EAutonomyState::E_IN_STOP:
			handleInStopEvent();
			break;
		case communication::EAutonomyState::E_OBSTACLE:
			handleObstacleEvent();
		case communication::EAutonomyState::E_ERROR:
			handleErrorEvent();
			break;
	}
	request();
}

void SimVehicle::handleIdleEvent() {
	std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
}

void SimVehicle::handleDriveEvent() {
	if(checkForStop()) { return; }

	if(driveMillisecondLeft_ < globalContext_->settings->messagePeriodMs) {
		std::this_thread::sleep_for(std::chrono::milliseconds(driveMillisecondLeft_));
	} else {
		std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
	}
	driveMillisecondLeft_ -= globalContext_->settings->messagePeriodMs;

	if(driveMillisecondLeft_ <= 0) {
		if(changeRoute_) {
			changeRoute();
		}
		setNextPosition();
	}
}

void SimVehicle::handleInStopEvent() {
	if(inStopMillisecondsLeft_ > globalContext_->settings->messagePeriodMs) {
		std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
	} else {
		std::this_thread::sleep_for(std::chrono::milliseconds(inStopMillisecondsLeft_));
	}
	inStopMillisecondsLeft_ -= globalContext_->settings->messagePeriodMs;
	if(inStopMillisecondsLeft_ < 0) {
		inStopMillisecondsLeft_ = 0;
	}
}

void SimVehicle::handleObstacleEvent() {
	std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
	settings::Logger::logWarning("Cars state is obstacle, this state is not supported.");
}

void SimVehicle::handleErrorEvent() {
	std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
	settings::Logger::logWarning("Car is in error state.");
}

void SimVehicle::setNextPosition() {
	actualPosition_ = actualRoute_->getPosition();
	actualRoute_->setNextPosition();
	nextPosition_ = actualRoute_->getPosition();

	actualSpeed_ = state_ == communication::EAutonomyState::E_DRIVE ? actualPosition_->getSpeedInMetersPerSecond() : 0;
	driveMillisecondLeft_ = common_utils::CommonUtils::timeToDriveInMilliseconds(
			common_utils::CommonUtils::calculateDistanceInMeters(*actualPosition_, *nextPosition_),
			actualSpeed_);

	if(state_ == communication::EAutonomyState::E_DRIVE) {
		settings::Logger::logInfo("{} route, distance to drive: {:.2f}m, time to get there: {:.2f}s", actualRouteName_,
								 common_utils::CommonUtils::calculateDistanceInMeters(*actualPosition_, *nextPosition_),
								 (double)driveMillisecondLeft_/1000);
	}
}

void SimVehicle::request() {
	communication::Status status;
	status.setLongitude(actualPosition_->getLongitude());
	status.setLatitude(actualPosition_->getLatitude());
	status.setSpeed(actualSpeed_);
	status.setState(state_);
	status.setNextStop(nextStop_);

	std::stringstream is;
	is << status;
	settings::Logger::logInfo("Sending status {}", is.str());
	com_->makeRequest(status);
	evaluateCommand();
}

void SimVehicle::evaluateCommand() {
	auto command = com_->getCommand();
	switch (command.getAction()) {
		case communication::EAutonomyAction::E_START:
			if(command.getMission().empty()) {
				updateVehicleState(communication::EAutonomyState::E_IDLE);
				return;
			}

			if(command.getRoute() != actualRouteName_ && !command.getRoute().empty()) {
				auto nextRoute = map_.getRoute(command.getRoute());
				if(!nextRoute) {
					settings::Logger::logWarning("Route {} was not found. Command will be ignored", command.getRoute());
					return;
				}
				if(!changeRoute_) {
					settings::Logger::logInfo("New route received.");
				}
				changeRoute_ = true;
				nextRouteName_ = command.getRoute();
			}

			if(checkStations_) {
				actualRoute_->compareStations(command.getMission());
				checkStations_ = false;
			}

			if(!common_utils::CommonUtils::compareMissions(mission_, command.getMission())) {
				if(!changeRoute_) {
					if(!actualRoute_->areStopsPresent(command.getMission())) {
						settings::Logger::logWarning(
								"Received stopNames are not on route, stopNames will be completely ignored {}",
								common_utils::CommonUtils::constructMissionString(mission_));
						mission_.clear();
						missionValidity_ = false;
						return;
					}
				}
				missionValidity_ = true;
				mission_ = command.getMission();
			}

			if(state_ == communication::EAutonomyState::E_IN_STOP) {
				if(inStopMillisecondsLeft_ == 0) {
					if(mission_.empty()) {
						updateVehicleState(communication::EAutonomyState::E_IDLE);
					} else {
						updateVehicleState(communication::EAutonomyState::E_DRIVE);
					}
				} else {
					updateVehicleState(communication::EAutonomyState::E_IN_STOP);
				}
			} else {
				updateVehicleState(communication::EAutonomyState::E_DRIVE);
			}

			if(state_ != communication::EAutonomyState::E_IN_STOP) {
				nextStop_ = mission_.front();
			}
			break;

		case communication::EAutonomyAction::E_STOP:
		case communication::EAutonomyAction::E_NO_ACTION:
			updateVehicleState(communication::EAutonomyState::E_IDLE);
			break;

	default:
			settings::Logger::logWarning("Unknown action received: {}", common_utils::EnumUtils::enumToString(command.getAction()));
			break;
	}
}

bool SimVehicle::checkForStop() {
	if(actualPosition_->isStop() && actualPosition_->getName() == nextStop_.name) {
		updateVehicleState(communication::EAutonomyState::E_IN_STOP);
		settings::Logger::logInfo("Car have arrived at the stop {}", nextStop_.name);
		return true;
	}
	return false;
}

void SimVehicle::updateVehicleState(communication::EAutonomyState state) {
	if(!missionValidity_) {
		state_ = bringauto::communication::EAutonomyState::E_ERROR;
		return;
	}
	if(state_ == state) {
		return;
	}
#ifdef STATE_SMURF
	settings::StateSmurfDefinition::changeToState(globalContext_->transitions, state);
#endif
	state_ = state;
	switch(state_) {
		case communication::EAutonomyState::E_IDLE:
			nextStop_.name.clear();
			actualSpeed_ = 0;
			break;
		case communication::EAutonomyState::E_DRIVE:
			actualSpeed_ = actualPosition_->getSpeedInMetersPerSecond();
			driveMillisecondLeft_ = common_utils::CommonUtils::timeToDriveInMilliseconds(
					common_utils::CommonUtils::calculateDistanceInMeters(*actualPosition_, *nextPosition_), actualSpeed_);
			break;
		case communication::EAutonomyState::E_IN_STOP:
			actualSpeed_ = 0;
			inStopMillisecondsLeft_ = globalContext_->settings->stopWaitTime*1000;
			break;
		case communication::EAutonomyState::E_OBSTACLE:
		case communication::EAutonomyState::E_ERROR:
			break;
	}
}

void SimVehicle::changeRoute() {
	auto nextRoute = map_.getRoute(nextRouteName_);
	if(nextRoute) {
		std::shared_ptr<osm::Point> nextPosition = nextRoute->getClosestPoint(*actualPosition_);
		if(nextPosition) {
			actualRoute_ = nextRoute;
			actualRouteName_ = nextRouteName_;
			settings::Logger::logInfo("Route changed to: {}.", nextRouteName_);

			if(!actualRoute_->areStopsPresent(mission_)) {  // Check if all stops are present on the new route
				settings::Logger::logWarning(
						"Received stopNames are not on route, stopNames will be completely ignored {}",
						common_utils::CommonUtils::constructMissionString(mission_));
				mission_.clear();
				missionValidity_ = false;
				return;
			}

			changeRoute_ = false;
			checkStations_ = true;
			actualRoute_->setPositionAndDirection(*nextPosition, nextStop_.name);
		} else {
			settings::Logger::logInfo("Vehicle is not on a the new route and cannot switch routes yet");
		}
	} else {
		settings::Logger::logWarning("Route {} was not found.", nextRouteName_);
	}

}

}
