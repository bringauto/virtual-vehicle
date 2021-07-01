#include <bringauto/virtual_vehicle/Vehicle.hpp>
#include <bringauto/logging/Logger.hpp>
#include <osmium/geom/haversine.hpp>
#include <unistd.h>

void  bringauto::virtual_vehicle::Vehicle::initialize() {
    route_->prepareRoute();
}

void  bringauto::virtual_vehicle::Vehicle::drive() {
    setNextPosition();
    while (!globalContext_->ioContext.stopped()) {
        driveToNextPosition();
        com_->sendStatus(actualPosition->getLatitude(), actualPosition->getLongitude(),
                         actualPosition->getSpeedInMetersPerSecond());
        waitIfInStop();
    }
}

void  bringauto::virtual_vehicle::Vehicle::driveToNextPosition() {
    double distance = distanceToNextPosition();
    double speedInMetersPerSecond = actualPosition->getSpeedInMetersPerSecond();
    double secondsToWait = ((distance / speedInMetersPerSecond));
    bringauto::logging::Logger::logInfo(
            "Distance: " + std::to_string(distance) + ", seconds to wait: " + std::to_string(secondsToWait));
    std::this_thread::sleep_for(std::chrono::duration<double>(secondsToWait));
    setNextPosition();
}

double  bringauto::virtual_vehicle::Vehicle::distanceToNextPosition() {
    return osmium::geom::haversine::distance(
            osmium::geom::Coordinates{actualPosition->getLatitude(), actualPosition->getLongitude()},
            osmium::geom::Coordinates{nextPosition->getLatitude(), nextPosition->getLongitude()});
}

void  bringauto::virtual_vehicle::Vehicle::setNextPosition() {
    actualPosition = route_->getPosition();
    route_->setNextPosition();
    nextPosition = route_->getPosition();
}

void  bringauto::virtual_vehicle::Vehicle::waitIfInStop() {
    if (actualPosition->isStop()) {
        bringauto::logging::Logger::logInfo("Car is waiting at the stop");
        std::this_thread::sleep_for(std::chrono::duration<double>(stopTimeSec_));
    }
}