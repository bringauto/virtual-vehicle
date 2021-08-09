#pragma once

#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <boost/asio.hpp>

namespace bringauto::communication {
    /**
     * Class defining interface for communication, it is meant to be derived from and implemented by specialized classes
     * using specific connection like proto buffer
     */
    class ICommunication {
    public:
        /**
         * States of car that are used in messages
         */
        enum State {
            IDLE = 0, DRIVE = 1, IN_STOP = 2
        };

        /**
         * Initialize connection to the whatever needs status data
         */
        virtual void initializeConnection() {};

        /**
         * Send vehicle position and speed
         * @param lon longitude position
         * @param lat latitude position
         * @param speed speed of vehicle
         */
        virtual void sendStatus(double lon, double lat, double speed, State state, const std::string &nextStop) {};

        /**
         * Structure for storing command information, it contains list of stops to go (orders) and Action command
         */
        struct Command {
            std::vector<std::string> stops;
            enum Action {
                NO_ACTION = 0,
                STOP = 1,
                START = 2
            } action = Action::NO_ACTION;
        };

        /**
         * See if new command was received
         * @return true if new command was received
         */
        bool isNewCommand() { return isNewCommand_; };

        /**
         * Get latest command
         * @return command structure
         */
        virtual const Command &getCommand() {
            return command_;
        };

        virtual ~ICommunication() = default;

    protected:
        Command command_;
        bool isNewCommand_{false};
    };


}