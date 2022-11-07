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
	updateVehicleState(communication::Status::IDLE);
	com_->initializeConnection();

	setNextPosition();
}

void SimVehicle::nextEvent() {
	switch(state_) {
		case communication::Status::IDLE:
			handleIdleEvent();
			break;
		case communication::Status::DRIVE:
			handleDriveEvent();
			break;
		case communication::Status::IN_STOP:
			handleInStopEvent();
			break;
		case communication::Status::OBSTACLE:
			handleObstacleEvent();
		case communication::Status::ERROR:
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

	actualSpeed_ = state_ == communication::Status::DRIVE ? actualPosition_->getSpeedInMetersPerSecond() : 0;
	driveMillisecondLeft_ = common_utils::CommonUtils::timeToDriveInMilliseconds(
			common_utils::CommonUtils::calculateDistanceInMeters(actualPosition_, nextPosition_),
			actualSpeed_);

	if(state_ == communication::Status::DRIVE) {
		logging::Logger::logInfo("{} route, distance to drive: {:.2f}m, time to get there: {:.2f}s", actualRouteName_,
								 common_utils::CommonUtils::calculateDistanceInMeters(actualPosition_, nextPosition_),
								 (double)driveMillisecondLeft_/1000);
	}
}

void SimVehicle::request() {
	communication::Status status { actualPosition_->getLongitude(), actualPosition_->getLatitude(),
								   actualSpeed_, state_, nextStopName_ };
	std::stringstream is;
	is << status;
	logging::Logger::logInfo("Sending status {}", is.str());
	com_->makeRequest(status);
	evaluateCommand();
}

void SimVehicle::evaluateCommand() {
	auto command = com_->getCommand();
#ifdef STATE_SMURF
	settings::StateSmurfDefinition::changeToState(globalContext_->transitions, command.action);
#endif

	if(command.stops.empty()) {
		updateVehicleState(communication::Status::IDLE);
		return;
	}
	//TODO novy if pro kontrolu routy, mozna jen prepinac
	if(checkStations_) {
		actualRoute_->compareStations(command.routeStations);
		checkStations_ = false;
	}

	if(command.route != actualRouteName_ && !command.route.empty()) {
		if(!changeRoute_) {
			logging::Logger::logInfo("New route received.");
		}
		changeRoute_ = true;
		nextRouteName_ = command.route;
	}
	if(mission_ != command.stops) {
		if(!changeRoute_) {

			if(!actualRoute_->areStopsPresent(command.stops)) {
				logging::Logger::logWarning(
						"Received stopNames are not on route, stopNames will be completely ignored {}",
						common_utils::CommonUtils::constructMissionString(mission_));
				mission_.clear(); // TODO do we want to clear mission when new command is invalid???
				missionValidity_ = false;
				return;
			}
		}
		missionValidity_ = true;
		mission_ = command.stops;
	}

	switch(state_) {
		case communication::Status::IDLE:
			if(command.action == communication::Command::START) {
				updateVehicleState(communication::Status::DRIVE);
			} else {
				updateVehicleState(communication::Status::IDLE);
			}
			break;
		case communication::Status::DRIVE:
			if(command.action == communication::Command::STOP) {
				updateVehicleState(communication::Status::IDLE);
			} else {
				updateVehicleState(communication::Status::DRIVE);
			}
			break;
		case communication::Status::IN_STOP:
			if(command.action == communication::Command::START) {
				if(inStopMillisecondsLeft_ == 0) {
					if(mission_.empty()) {
						updateVehicleState(communication::Status::IDLE);
					} else {
						updateVehicleState(communication::Status::DRIVE);
					}
				} else {
					updateVehicleState(communication::Status::IN_STOP);
				}
			} else {
				updateVehicleState(communication::Status::IDLE);
			}
			break;
		case communication::Status::OBSTACLE:
		case communication::Status::ERROR:
			break;
	}

	if(state_ != communication::Status::IN_STOP) {
		nextStopName_ = mission_.front();
	}
}

int SimVehicle::checkForStop() {
	if(actualPosition_->isStop() && actualPosition_->getName() == nextStopName_) {
		updateVehicleState(communication::Status::State::IN_STOP);
		bringauto::logging::Logger::logInfo("Car have arrived at the stop {}", nextStopName_);
		return true;
	}
	return false;
}

void SimVehicle::updateVehicleState(communication::Status::State state) {
	if(!missionValidity_) {
		state_ = bringauto::communication::Status::State::ERROR;
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
		case communication::Status::IDLE:
			nextStopName_.clear();
			actualSpeed_ = 0;
			break;
		case communication::Status::DRIVE:
			actualSpeed_ = actualPosition_->getSpeedInMetersPerSecond();
			driveMillisecondLeft_ = common_utils::CommonUtils::timeToDriveInMilliseconds(
					common_utils::CommonUtils::calculateDistanceInMeters(actualPosition_, nextPosition_), actualSpeed_);
			break;
		case communication::Status::IN_STOP:
			actualSpeed_ = 0;
			inStopMillisecondsLeft_ = globalContext_->settings->stopWaitTime*1000;
			break;
		case communication::Status::OBSTACLE:
		case communication::Status::ERROR:
			break;
	}
}

void SimVehicle::changeRoute() {
	auto nextRoute = map_.getRoute(nextRouteName_);
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
		actualRoute_->setPositionAndDirection(*actualPosition_, nextStopName_);
	} else {
		logging::Logger::logInfo("Vehicle is not on a the new route and cannot switch routes yet");
	}
}
}