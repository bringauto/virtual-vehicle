#pragma once

#include <bringauto/communication/ICommunication.hpp>

namespace bringauto::communication {
    class TerminalOutput : public ICommunication {
    public:
        void initializeConnection() override;

        /**
         * Print status to standard output
         * @param lon
         * @param lat
         * @param speed
         */
        void sendStatus(double lon, double lat, double speed) override;

        ~TerminalOutput() override;
    };
}