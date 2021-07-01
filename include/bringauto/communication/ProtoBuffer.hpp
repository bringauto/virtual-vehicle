#pragma once

#include <bringauto/communication/ICommunication.hpp>
#include <boost/asio.hpp>

namespace bringauto::communication {
    class ProtoBuffer : public ICommunication {
    public:
        ProtoBuffer(const std::string& ipAddress, int port) : ipAddress_(ipAddress), port_(port), socket_(io_service_) {};

        /**
         * Establishes connection with server via tcp socket (boost::asio)
         */
        void initializeConnection() override;

        /**
         * Send car status using protocol buffers and tcp to server
         */
        void sendStatus(double lat, double lon, double speed) override;

        ~ProtoBuffer() override;

    private:
        boost::asio::io_service io_service_{};
        boost::asio::ip::tcp::socket socket_;
        std::string ipAddress_;
        int port_;
    };
}
