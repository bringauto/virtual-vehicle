#include <bringauto/virtual_vehicle/vehicle_provider/SimVehicle.hpp>
#include <bringauto/common_utils/CommonUtils.hpp>

#include <bringauto/logging/Logger.hpp>

#include <thread>



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
	logging::Logger::logWarning("Cars state is obstacle, this state is not supported.");
}

void SimVehicle::handleErrorEvent() {
	std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
	logging::Logger::logWarning("Car is in error state.");
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
		logging::Logger::logInfo("{} route, distance to drive: {:.2f}m, time to get there: {:.2f}s", actualRouteName_,
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
	logging::Logger::logInfo("Sending status {}", is.str());
	com_->makeRequest(status);
	evaluateCommand();
}

void SimVehicle::evaluateCommand() {
	auto command = com_->getCommand();
#ifdef STATE_SMURF
	settings::StateSmurfDefinition::changeToState(globalContext_->transitions, command.getAction());
#endif
	if(command.getRoute() != actualRouteName_ && !command.getRoute().empty()) {
		auto nextRoute = map_.getRoute(command.getRoute());
		if(!nextRoute) {
			logging::Logger::logWarning("Route {} was not found. Command will be ignored", command.getRoute());
			return;
		}
		if(!changeRoute_) {
			logging::Logger::logInfo("New route received.");
		}
		changeRoute_ = true;
		nextRouteName_ = command.getRoute();
	}

	if(command.getMission().empty()) {
		updateVehicleState(communication::EAutonomyState::E_IDLE);
		return;
	}

	if(checkStations_) {
		actualRoute_->compareStations(command.getMission());
		checkStations_ = false;
	}


	if(!common_utils::CommonUtils::compareMissions(mission_, command.getMission())) {
		if(!changeRoute_) {
			if(!actualRoute_->areStopsPresent(command.getMission())) {
				logging::Logger::logWarning(
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

	switch(state_) {
		case communication::EAutonomyState::E_IDLE:
			if(command.getAction() == communication::EAutonomyAction::E_START) {
				updateVehicleState(communication::EAutonomyState::E_DRIVE);
			} else {
				updateVehicleState(communication::EAutonomyState::E_IDLE);
			}
			break;
		case communication::EAutonomyState::E_DRIVE:
			if(command.getAction() == communication::EAutonomyAction::E_STOP) {
				updateVehicleState(communication::EAutonomyState::E_IDLE);
			} else {
				updateVehicleState(communication::EAutonomyState::E_DRIVE);
			}
			break;
		case communication::EAutonomyState::E_IN_STOP:
			if(command.getAction() == communication::EAutonomyAction::E_START) {
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
				updateVehicleState(communication::EAutonomyState::E_IDLE);
			}
			break;
		case communication::EAutonomyState::E_OBSTACLE:
		case communication::EAutonomyState::E_ERROR:
			break;
	}

	if(state_ != communication::EAutonomyState::E_IN_STOP) {
		nextStop_ = mission_.front();
	}
}

bool SimVehicle::checkForStop() {
	if(actualPosition_->isStop() && actualPosition_->getName() == nextStop_.name) {
		updateVehicleState(communication::EAutonomyState::E_IN_STOP);
		bringauto::logging::Logger::logInfo("Car have arrived at the stop {}", nextStop_.name);
		return true;
	}
	return false;
}

void SimVehicle::updateVehicleState(communication::EAutonomyState state) {
	if(!missionValidity_) {
		state_ = bringauto::communication::EAutonomyState::E_ERROR;
		return;
	}
#ifdef STATE_SMURF
	settings::StateSmurfDefinition::changeToState(globalContext_->transitions, state);
#endif
	if(state_ == state) {
		return;
	}
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
		if(nextRoute->isPointPresent(*actualPosition_)) {
			actualRoute_->setNextPosition();
			actualRoute_ = nextRoute;
			actualRouteName_ = nextRouteName_;
			logging::Logger::logInfo("Route changed to: {}.", nextRouteName_);

			if(!actualRoute_->areStopsPresent(mission_)) {  // Check if all stops are present on the new route
				logging::Logger::logWarning(
						"Received stopNames are not on route, stopNames will be completely ignored {}",
						common_utils::CommonUtils::constructMissionString(mission_));
				mission_.clear();
				missionValidity_ = false;
				return;
			}

			changeRoute_ = false;
			checkStations_ = true;
			actualRoute_->setPositionAndDirection(*actualPosition_, nextStop_.name);
		} else {
			logging::Logger::logInfo("Vehicle is not on a the new route and cannot switch routes yet");
		}
	} else {
		logging::Logger::logWarning("Route {} was not found.", nextRouteName_);
	}

}

}