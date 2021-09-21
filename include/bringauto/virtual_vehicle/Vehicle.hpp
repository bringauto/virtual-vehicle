#pragma once

#include <bringauto/osm/OsmiumHandler.hpp>
#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <utility>

namespace bringauto::virtual_vehicle {
    class Vehicle {
    public:
        Vehicle(std::shared_ptr<bringauto::osm::Route> route,
                std::shared_ptr<bringauto::communication::ICommunication> com,
                std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> globalContext) : route_(std::move(route)),
                                                                                            com_(std::move(com)),
                                                                                            globalContext_(std::move(
                                                                                                    globalContext)) {};

        /**
         * Prepare vehicle and route for drive simulation
         */
        void initialize();

        /**
         * Simulate vehicle driving
         */
        void drive();

    private:
        std::shared_ptr<bringauto::osm::Route> route_;
        std::shared_ptr<bringauto::communication::ICommunication> com_;
        std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> globalContext_;

        std::shared_ptr<bringauto::osm::Point> actualPosition_;
        std::shared_ptr<bringauto::osm::Point> nextPosition_;
        double actualSpeed_{0}; //m/s
        std::vector<std::string> mission_;
        bool missionValidity_{true};
        std::string nextStopName_;

        bringauto::communication::ICommunication::State state_{bringauto::communication::ICommunication::State::IDLE};

        /**
         * Vehicle will simulate driving from current point to next point
         */
        void driveToNextPosition();

        /**
         * Method will calculate and return distance between actual point and next point
         * @return distance to next point on route in meters
         */
        double distanceToNextPosition();

        /**
         * Method will wait for stopTimeSec_ if actualPosition_ is a stop
         */
        void waitIfStopOrIdle();

        void sendVehicleStatus();

        void evaluateCommand();

        bool isChangeInMission(const std::vector<std::string> &stopNames);

        void setNextStop();

        void updateVehicleState(bringauto::communication::ICommunication::State state);

        void checkForStop();

        void setVehicleActionFromCommand(bringauto::communication::ICommunication::Command::Action action);

        void updateMissionFromCommand(const std::vector<std::string> &mission);

        std::string constructMissionString();
    };
}

