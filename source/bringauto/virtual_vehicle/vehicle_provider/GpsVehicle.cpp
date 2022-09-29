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

    actualRoute_ = map_.getAllRoutes()[0]; /// Make actualRoute_ the first in the vector for initialization reason

	switch(globalContext_->settings->gpsProvider) {
		case settings::GpsProvider::INVALID:
			throw std::runtime_error("Unknown gps provider!");
		case settings::GpsProvider::RUTX09:
			gpsProvider_ = std::make_unique<gps_provider::RUTX09>(globalContext_->settings->rutxIp,
																  globalContext_->settings->rutxPort,
																  globalContext_->settings->rutxSlaveId);
			break;
		case settings::GpsProvider::UBLOX:
			gpsProvider_ = std::make_unique<gps_provider::UBlocks>();
			break;
		case settings::GpsProvider::MAP:
			gpsProvider_ = std::make_unique<gps_provider::MapGps>(actualRoute_); //TODO nevim co to bere
	}
	status_.state = communication::Status::IDLE;
	com_->initializeConnection();
	eventDelayInSec_ = ((double)globalContext_->settings->messagePeriodMs)/1000;
	stops_ = actualRoute_->getStops();
}

void GpsVehicle::nextEvent() {
	updatePosition();
	makeRequest();

	std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
}

void GpsVehicle::updatePosition() {
	auto position = gpsProvider_->getPosition();
	double lastLatitude = status_.latitude;
	double lastLongitude = status_.longitude;
	status_.latitude = position.latitude;
	status_.longitude = position.longitude;
	auto speed = gpsProvider_->getSpeed();
	if(currentStop_){
		auto distanceToStop = common_utils::CommonUtils::calculateDistanceInMeters(currentStop_->getLatitude(), currentStop_->getLongitude(), status_.latitude, status_.longitude);
		if(distanceToStop < globalContext_->settings->stopRadius){
			status_.state = communication::Status::IN_STOP;
			logging::Logger::logInfo("Car arrived at stop {}.", currentStop_->getName());
		}
	}
	status_.speed = common_utils::CommonUtils::calculateDistanceInMeters(lastLatitude, lastLongitude, status_.latitude, status_.longitude)/eventDelayInSec_;
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
	if(command.stops.empty()) {
		status_.state = communication::Status::IDLE;
		status_.nextStop = "";
		currentStop_ = nullptr;
	} else {
		status_.state = communication::Status::DRIVE;
		status_.nextStop = command.stops.front();
		currentStop_ = nullptr;
		for(const auto& stop: stops_){
			if(stop->getName() == status_.nextStop){
				currentStop_ = stop;
			}
		}
		if(!currentStop_){
			logging::Logger::logWarning("Received stop with name {} but stop was not found on map.", status_.nextStop);
		}
	}
}

}