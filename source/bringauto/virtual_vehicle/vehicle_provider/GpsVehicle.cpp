#include <bringauto/virtual_vehicle/vehicle_provider/GpsVehicle.hpp>
#include <bringauto/virtual_vehicle/gps_provider/Rutx09.hpp>
#include <bringauto/virtual_vehicle/gps_provider/UBlocks.hpp>
#include <bringauto/communication/Status.hpp>
#include <bringauto/common_utils/CommonUtils.hpp>

#include <bringauto/logging/Logger.hpp>



namespace bringauto::virtual_vehicle::vehicle_provider {

void GpsVehicle::initialize() {
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
	}
	status_.state = communication::Status::IDLE;
	com_->initializeConnection();
	eventDelayInSec_ = ((double)globalContext_->settings->messagePeriodMs)/1000;
}

void GpsVehicle::nextEvent() {
	updatePosition();
	makeRequest();

	std::this_thread::sleep_for(std::chrono::milliseconds(globalContext_->settings->messagePeriodMs));
}

void GpsVehicle::updatePosition() {
	double lastLatitude = status_.latitude;
	double lastLongitude = status_.longitude;
	status_.latitude = gpsProvider_->getLatitude();
	status_.longitude = gpsProvider_->getLongitude();
	auto speed = gpsProvider_->getSpeed();
	status_.speed = common_utils::CommonUtils::calculateDistanceInMeters(lastLatitude, lastLongitude, status_.latitude, status_.longitude)/eventDelayInSec_;
	std::cout << "calculated: " << status_.speed << " read: " << speed << std::endl;
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
	} else {
		status_.state = communication::Status::DRIVE;
		status_.nextStop = command.stops.front();
	}
}

}