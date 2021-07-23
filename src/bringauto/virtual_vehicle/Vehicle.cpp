#include <bringauto/virtual_vehicle/Vehicle.hpp>
#include <bringauto/logging/Logger.hpp>

#include <osmium/geom/haversine.hpp>

void bringauto::virtual_vehicle::Vehicle::initialize() {
    route_->prepareRoute();
    actualPosition_ = route_->getPosition();
    route_->setNextPosition();
    nextPosition_ = route_->getPosition();
    checkForStop();
}

void bringauto::virtual_vehicle::Vehicle::drive() {
    while (!globalContext_->ioContext.stopped()) {
        evaluateCommand();
        if (state_ == bringauto::communication::ICommunication::State::DRIVE) {
            driveToNextPosition();
            checkForStop();
        }
        sendVehicleStatus();
        waitIfStopOrIdle();
    }
}

void bringauto::virtual_vehicle::Vehicle::driveToNextPosition() {
    double distance = distanceToNextPosition();
    double speedInMetersPerSecond = actualPosition_->getSpeedInMetersPerSecond();
    double secondsToWait = ((distance / speedInMetersPerSecond));
    bringauto::logging::Logger::logInfo(
            "Distance: " + std::to_string(distance) + ", seconds to wait: " + std::to_string(secondsToWait));

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
    switch(state_){
        case bringauto::communication::ICommunication::State::IN_STOP:
            setNextStop();
        case bringauto::communication::ICommunication::State::IDLE:
            //todo sleep until new command
            std::this_thread::sleep_for(std::chrono::duration<double>(globalContext_->stopWaitSeconds));
            break;
        default:
            break;
    }
}

void bringauto::virtual_vehicle::Vehicle::sendVehicleStatus() {
    com_->sendStatus(actualPosition_->getLatitude(), actualPosition_->getLongitude(),
                     actualSpeed_, state_, nextStopName_);
}

void bringauto::virtual_vehicle::Vehicle::evaluateCommand() {
    auto command = com_->getCommand();
    updateVehicleStopsFromCommand(command.stops);
    setVehicleActionFromCommand(command.action);
    checkForStop();
}

bool bringauto::virtual_vehicle::Vehicle::isChangeInStops(const std::vector<std::string> &stopNames) {
    return stopNameList_ != stopNames;
}

void bringauto::virtual_vehicle::Vehicle::setNextStop() {
    if(stopNameList_.empty()){
        return;
    }

    stopNameList_.erase(stopNameList_.begin());

    if(stopNameList_.empty()){
        nextStopName_.clear();
        if(globalContext_->cruise){
            logging::Logger::logInfo("Car have fulfilled all its orders, car will continue cruising until the end of universe...or this simulation");
        }else{
            logging::Logger::logInfo("Car have fulfilled all its orders, awaiting next command");
        }
    }else{
        nextStopName_ = stopNameList_.front();
        logging::Logger::logInfo("Driving to next stop: " + nextStopName_);
    }
}

void bringauto::virtual_vehicle::Vehicle::checkForStop() {
    if(stopNameList_.empty()){
        return;
    }
    if (actualPosition_->isStop() && actualPosition_->getName() == nextStopName_) {
        updateVehicleState(bringauto::communication::ICommunication::State::IN_STOP);
        bringauto::logging::Logger::logInfo("Car have arrived at the stop " + nextStopName_);
    }
}

void bringauto::virtual_vehicle::Vehicle::setVehicleActionFromCommand(
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
            if(stopNameList_.empty() && !globalContext_->cruise){
                logging::Logger::logInfo("Order list is empty and cruise is turned off, ignoring START action, setting IDLE");
                updateVehicleState(bringauto::communication::ICommunication::State::IDLE);
            }else{
                updateVehicleState(bringauto::communication::ICommunication::State::DRIVE);
            }
            break;
        default:
            logging::Logger::logWarning("Warning unknown command" + std::to_string(action));
            break;
    }
}

void bringauto::virtual_vehicle::Vehicle::updateVehicleStopsFromCommand(const std::vector<std::string>& stopNames) {
    if (isChangeInStops(stopNames)) {
        stopNameList_ = stopNames;
        nextStopName_.clear();
        if(stopNameList_.empty()){
            logging::Logger::logWarning("Received empty stop list");
            return;
        }
        if(!route_->areStopsPresent(stopNameList_)){
            logging::Logger::logWarning("Received stopNames are not on route, stopNames will be completely ignored");
            return;
        }
        nextStopName_ = stopNameList_.front();
        logging::Logger::logInfo("List of stops have been changed, next stop: " + nextStopName_);
    }
}

void bringauto::virtual_vehicle::Vehicle::updateVehicleState(bringauto::communication::ICommunication::State state) {
    using bringauto::communication::ICommunication;
    state_ = state;

    if(state_ == ICommunication::State::IDLE || state_ == ICommunication::State::IN_STOP){
        actualSpeed_ = 0;
    }else{
        actualSpeed_ = actualPosition_->getSpeedInMetersPerSecond();
    }
}
