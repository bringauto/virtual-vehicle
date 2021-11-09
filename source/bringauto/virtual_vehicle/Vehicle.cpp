#include <bringauto/virtual_vehicle/Vehicle.hpp>

#include <bringauto/logging/Logger.hpp>
#include <osmium/geom/haversine.hpp>

void bringauto::virtual_vehicle::Vehicle::initialize() {
	route_->prepareRoute();
	actualPosition_ = route_->getPosition();
	route_->setNextPosition();
	nextPosition_ = route_->getPosition();
	checkForStop();
	updateVehicleStatus();
	com_->startWorker();
}

void bringauto::virtual_vehicle::Vehicle::drive() {
	while (!globalContext_->ioContext.stopped()) {
		evaluateCommand();
		if (state_ == bringauto::communication::ICommunication::State::DRIVE) {
			driveToNextPosition();
			checkForStop();
		}
		updateVehicleStatus();
		waitIfStopOrIdle();
	}
}

void bringauto::virtual_vehicle::Vehicle::driveToNextPosition() {
	double distance = distanceToNextPosition();
	double speedInMetersPerSecond = actualPosition_->getSpeedInMetersPerSecond();
	double secondsToWait = ((distance / speedInMetersPerSecond));
	bringauto::logging::Logger::logInfo(
			"Distance to drive: {:.2f}m, time to get there: {:.2f}s", distance, secondsToWait);

	std::this_thread::sleep_for(std::chrono::duration<double>(secondsToWait));

	actualPosition_ = route_->getPosition();
	route_->setNextPosition();
	nextPosition_ = route_->getPosition();
}

double bringauto::virtual_vehicle::Vehicle::distanceToNextPosition() {
	return osmium::geom::haversine::distance(
			osmium::geom::Coordinates{actualPosition_->getLatitude(), actualPosition_->getLongitude()},
			osmium::geom::Coordinates{nextPosition_->getLatitude(), nextPosition_->getLongitude()});
}

void bringauto::virtual_vehicle::Vehicle::waitIfStopOrIdle() {
	switch (state_) {
		case bringauto::communication::ICommunication::State::IN_STOP:
			std::this_thread::sleep_for(std::chrono::duration<double>(globalContext_->stopWaitSeconds));
			setNextStop();
			break;
		case bringauto::communication::ICommunication::State::IDLE:
			std::this_thread::sleep_for(std::chrono::duration<double>(globalContext_->stopWaitSeconds));
			break;
		case bringauto::communication::ICommunication::State::ERROR:
			std::this_thread::sleep_for(std::chrono::duration<double>(globalContext_->stopWaitSeconds));
		default:
			break;
	}
}

void bringauto::virtual_vehicle::Vehicle::updateVehicleStatus() {
	com_->updateStatus(actualPosition_->getLatitude(), actualPosition_->getLongitude(),
					   actualSpeed_, state_, nextStopName_);
}

void bringauto::virtual_vehicle::Vehicle::evaluateCommand() {
	if (com_->isNewCommand()) {
		auto command = com_->getCommand();
		updateMissionFromCommand(command.stops);
		setVehicleStateFromCommand(command.action);
		checkForStop();
	}
}

bool bringauto::virtual_vehicle::Vehicle::isChangeInMission(const std::vector<std::string> &mission) {
	return mission_ != mission;
}

void bringauto::virtual_vehicle::Vehicle::setNextStop() {
	if (mission_.empty()) {
		return;
	}

	mission_.erase(mission_.begin());

	if (mission_.empty()) {
		nextStopName_.clear();
		if (globalContext_->cruise) {
			logging::Logger::logInfo(
					"Car have fulfilled all its orders, car will continue cruising until the end of universe...or this simulation");
		} else {
			logging::Logger::logInfo("Car have fulfilled all its orders, awaiting next command");
		}
	} else {
		if (nextStopName_ == mission_.front()) { //fixing two same stops after each other
			updateVehicleState(bringauto::communication::ICommunication::State::DRIVE);
			updateVehicleStatus();
		}
		nextStopName_ = mission_.front();
		logging::Logger::logInfo("Driving to next stop: {}", nextStopName_);
	}
}

void bringauto::virtual_vehicle::Vehicle::checkForStop() {
	if (mission_.empty()) {
		return;
	}
	if (actualPosition_->isStop() && actualPosition_->getName() == nextStopName_) {
		updateVehicleState(bringauto::communication::ICommunication::State::IN_STOP);
		bringauto::logging::Logger::logInfo("Car have arrived at the stop {}", nextStopName_);
	}
}

void bringauto::virtual_vehicle::Vehicle::setVehicleStateFromCommand(
		bringauto::communication::ICommunication::Command::Action action) {

	switch (action) {
		case bringauto::communication::ICommunication::Command::Action::NO_ACTION:
			logging::Logger::logInfo("No action command received");
			break;
		case bringauto::communication::ICommunication::Command::Action::STOP:
			updateVehicleState(bringauto::communication::ICommunication::State::IDLE);
			logging::Logger::logInfo("Stop command received");
			break;
		case bringauto::communication::ICommunication::Command::Action::START:
			if (mission_.empty() && !globalContext_->cruise) {
				logging::Logger::logInfo(
						"Order list is empty and cruise is turned off, ignoring START action, setting IDLE");
				updateVehicleState(bringauto::communication::ICommunication::State::IDLE);
			} else {
				updateVehicleState(bringauto::communication::ICommunication::State::DRIVE);
			}
			break;
		default:
			logging::Logger::logWarning("Warning unknown command {}", action);
			break;
	}
}

void bringauto::virtual_vehicle::Vehicle::updateMissionFromCommand(const std::vector<std::string> &mission) {
	if (isChangeInMission(mission)) {
		missionValidity_ = true;
		mission_ = mission;
		nextStopName_.clear();
		if (mission_.empty()) {
			logging::Logger::logWarning("Received empty mission");
			return;
		}
		std::string missionString = constructMissionString();
		if (!route_->areStopsPresent(mission_)) {
			logging::Logger::logWarning(
					"Received stopNames are not on route, stopNames will be completely ignored {}", missionString);
			missionValidity_ = false;
			return;
		}
		nextStopName_ = mission_.front();
		logging::Logger::logInfo("List of stops have been changed, new mission: {}", missionString);
	}
}

void bringauto::virtual_vehicle::Vehicle::updateVehicleState(bringauto::communication::ICommunication::State state) {
	using bringauto::communication::ICommunication;
	if (!missionValidity_) {
		state_ = bringauto::communication::ICommunication::State::ERROR;
		return;
	}

	state_ = state;

	if (state_ == ICommunication::State::IDLE || state_ == ICommunication::State::IN_STOP) {
		actualSpeed_ = 0;
	} else {
		actualSpeed_ = actualPosition_->getSpeedInMetersPerSecond();
	}
}

std::string bringauto::virtual_vehicle::Vehicle::constructMissionString() {
	std::string mission{"["};
	for (const auto &stopName: mission_) {
		mission += "\"" + stopName + "\", ";
	}
	mission += "]";
	return mission;
}