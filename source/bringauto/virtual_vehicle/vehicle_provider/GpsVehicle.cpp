#include <bringauto/virtual_vehicle/vehicle_provider/GpsVehicle.hpp>
#include <bringauto/virtual_vehicle/gps_provider/Rutx09.hpp>
#include <bringauto/virtual_vehicle/gps_provider/UBlocks.hpp>
#include <bringauto/virtual_vehicle/gps_provider/MapGps.hpp>
#include <bringauto/communication/Status.hpp>
#include <bringauto/common_utils/CommonUtils.hpp>

#include <bringauto/logging/Logger.hpp>

#include <thread>



namespace bringauto::virtual_vehicle::vehicle_provider {

void GpsVehicle::initialize() {
	map_.loadMapFromFile(globalContext_->settings->mapFilePath);
	if(globalContext_->settings->speedOverride) {
		map_.speedOverride(globalContext_->settings->speedOverrideMS);
	}
	map_.prepareRoutes();

	if(!globalContext_->settings->routeName.empty()) { /// Override default route by argument
		actualRoute_ = map_.getRoute(globalContext_->settings->routeName);
	} else { /// Make actualRoute_ the first in the vector for initialization reason
		actualRoute_ = map_.getAllRoutes()[0];
	}
	stops_ = actualRoute_->getStops();

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
			gpsProvider_ = std::make_unique<gps_provider::MapGps>(actualRoute_);
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
	if(currentStop_ != nullptr) {
		auto distanceToStop = common_utils::CommonUtils::calculateDistanceInMeters(currentStop_->getLatitude(),
																				   currentStop_->getLongitude(),
																				   status_.getLatitude(),
																				   status_.getLongitude());
		if(distanceToStop < globalContext_->settings->stopRadius) {
			status_.setState(communication::EAutonomyState::E_IN_STOP);
			logging::Logger::logInfo("Car arrived at stop {}.", currentStop_->getName());
		}
	}
	status_.setSpeed(
			common_utils::CommonUtils::calculateDistanceInMeters(lastLatitude, lastLongitude, status_.getLatitude(),
																 status_.getLongitude())/eventDelayInSec_);
}

void GpsVehicle::makeRequest() {
	com_->makeRequest(status_);
	std::stringstream is;
	is << status_;
	logging::Logger::logInfo("Sending status {}", is.str());
	evaluateCommand();
}

void GpsVehicle::evaluateCommand() {
	auto command = com_->getCommand();

	auto route = map_.getRoute(command.getRoute()); /// Change of route, need to update available stops on it
	if(!route) {
		logging::Logger::logWarning("Route {} was not found. Command will be ignored", command.getRoute());
		return;
	}

	actualRoute_ = route;
	stops_ = actualRoute_->getStops();

	if(command.getMission().empty()) {
		status_.setState(communication::EAutonomyState::E_IDLE);
		status_.setNextStop({});
		currentStop_ = nullptr;
	} else {
		status_.setState(communication::EAutonomyState::E_DRIVE);
		status_.setNextStop(command.getMission().front());
		for(const auto &stop: stops_) {
			if(stop->getName() == status_.getNextStop().name) {
				currentStop_ = stop;
				break;
			}
		}
		if(!currentStop_) {
			logging::Logger::logWarning("Received stop with name {} but stop was not found on map.",
										status_.getNextStop().name);
		}
	}
}

}