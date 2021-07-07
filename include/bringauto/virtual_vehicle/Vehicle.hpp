#pragma once

#include <bringauto/osm/OsmStructures.hpp>
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

        std::shared_ptr<bringauto::osm::Point> actualPosition;
        std::shared_ptr<bringauto::osm::Point> nextPosition;

        const int stopTimeSec_{10};

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
         * Method will change actual position to next position and get new next position
         */
        void setNextPosition();

        /**
         * Method will wait for stopTimeSec_ if actualPosition_ is a stop
         */
        void waitIfInStop();
    };
}

