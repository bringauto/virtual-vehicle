#pragma once

namespace bringauto::communication {
    class ICommunication {
    public:
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
        virtual void sendStatus(double lon, double lat, double speed) {};

        virtual ~ICommunication() = default;;
    };
}