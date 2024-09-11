#include <bringauto/virtual_vehicle/vehicle_provider/GpsVehicle.hpp>
#include <bringauto/virtual_vehicle/gps_provider/Rutx09.hpp>
#include <bringauto/virtual_vehicle/gps_provider/UBlocks.hpp>
#include <bringauto/virtual_vehicle/gps_provider/MapGps.hpp>
#include <bringauto/communication/Status.hpp>
#include <bringauto/common_utils/CommonUtils.hpp>

#include <bringauto/settings/LoggerId.hpp>

#include <thread>



namespace bringauto::virtual_vehicle::vehicle_provider {

void GpsVehicle::initialize() {
	switch(globalContext_->settings->gpsProvider) {
		case settings::GpsProvider::E_RUTX09:
			gpsProvider_ = std::make_unique<gps_provider::RUTX09>(globalContext_->settings->rutxIp,
																  globalContext_->settings->rutxPort,
																  globalContext_->settings->rutxSlaveId);
			break;
		case settings::GpsProvider::E_UBLOX:
			gpsProvider_ = std::make_unique<gps_provider::UBlocks>();
			break;
		case settings::GpsProvider::E_MAP:
//			gpsProvider_ = std::make_unique<gps_provider::MapGps>(actualRoute_); TODO update this
			break;
		default:
			throw std::runtime_error("Unknown gps provider!");

	}
	status_.setState(communication::EAutonomyState::E_IDLE);

	com_->initializeConnection();

	eventDelayInSec_ = ((double)globalContext_->settings->messagePeriodMs)/1000;
}

void GpsVehicle::nextEvent() {
	updatePosition();
	makeRequest();

	std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
}

void GpsVehicle::updatePosition() {
	auto position = gpsProvider_->getPosition();
	double lastLatitude = status_.getLatitude();
	double lastLongitude = status_.getLongitude();
	status_.setLatitude(position.latitude);
	status_.setLongitude(position.longitude);

	const auto &speed = common_utils::CommonUtils::calculateDistanceInMeters(
			lastLatitude, lastLongitude,
			status_.getLatitude(),status_.getLongitude()
			) / eventDelayInSec_;
	status_.setSpeed(speed);
	if (status_.getState() == communication::EAutonomyState::E_DRIVE) {
		checkForStop();
	} else if (status_.getState() == communication::EAutonomyState::E_IN_STOP) {
		auto distanceToStop = common_utils::CommonUtils::calculateDistanceInMeters(nextStop_->latitude,
																				   nextStop_->longitude,
																				   status_.getLatitude(),
																				   status_.getLongitude());
		if(distanceToStop < globalContext_->settings->stopRadiusM) { // TODO put drive }
		}
	}

}

void GpsVehicle::makeRequest() {
	com_->makeRequest(status_);
	std::stringstream is;
	is << status_;
	settings::Logger::logInfo("Sending status {}", is.str());
	evaluateCommand();
}

void GpsVehicle::evaluateCommand() {
	auto command = com_->getCommand();
	if(command.getMission().empty()) {
		status_.setState(communication::EAutonomyState::E_IDLE);
		status_.setNextStop({});
		nextStop_ = nullptr;
	} else {
		//	TODO change to DRIVE immidiately after IN_STOP??
		status_.setState(communication::EAutonomyState::E_DRIVE);
		nextStop_ = std::make_shared<osm::Route::Station>(command.getMission().front());
		status_.setNextStop(*nextStop_);
	}

	switch(status_.getState()) {
		case communication::EAutonomyState::E_IDLE:
			if(command.getAction() == communication::EAutonomyAction::E_START) {
				status_.setState(communication::EAutonomyState::E_DRIVE);
			}
			break;
		case communication::EAutonomyState::E_DRIVE:
			if(command.getAction() == communication::EAutonomyAction::E_STOP) {
				status_.setState(communication::EAutonomyState::E_IDLE);
			}
			break;
		case communication::EAutonomyState::E_IN_STOP:
			// TODO should it stay IN_STOP if nextStop null??
			if(command.getAction() != communication::EAutonomyAction::E_START /* || nextStop_ == nullptr */) {
				status_.setState(communication::EAutonomyState::E_IDLE);
			}
			break;
		case communication::EAutonomyState::E_OBSTACLE:
		case communication::EAutonomyState::E_ERROR:
		default:
			break;
	}
	// TODO when is the station removed from command? I need to act on it
//	if(status_.getState() != communication::EAutonomyState::E_IN_STOP) {
//		nextStop_ = mission_.front();
//	}

}

void GpsVehicle::checkForStop() {
	if(nextStop_ != nullptr) {
		auto distanceToStop = common_utils::CommonUtils::calculateDistanceInMeters(nextStop_->latitude,
																				   nextStop_->longitude,
																				   status_.getLatitude(),
																				   status_.getLongitude());
		if(distanceToStop < globalContext_->settings->stopRadiusM) {
//			bool lowSpeed = (speed < MAX_STOP_SPEED); TODO is it necessary to act according to speed?
			if (!timerRunning_) {
				settings::Logger::logInfo("The car is near station {}. Starting stop timer.", nextStop_->name);
				timerStart_ = std::chrono::steady_clock::now();
				timerRunning_ = true;
			}
			if(std::chrono::steady_clock::now() - timerStart_ >= globalContext_->settings->inStopDelayS) {
				status_.setState(communication::EAutonomyState::E_IN_STOP);
				settings::Logger::logInfo("Car arrived at stop {}.", nextStop_->name);
			}
		} else {
			settings::Logger::logInfo("Stop timer cancelled, the car has exited the station area.", nextStop_->name);
			timerRunning_ = false;
		}
	}
}

}