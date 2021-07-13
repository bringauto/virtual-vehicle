#pragma once

#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include "BringAutoDaemon.pb.h"

#include <boost/asio.hpp>

namespace bringauto::communication {
    /**
     * Specific communication interface implementation, uses proto buffer library to send and receive messages from server
     */
    class ProtoBuffer : public ICommunication {
        /**
         * Checking if proto buffer protocol and ICommunication protocol are the same
         */
        static_assert(static_cast<int>(State::IDLE) == static_cast<int>(BringAutoDaemon::CarStatus::IDLE));
        static_assert(static_cast<int>(State::DRIVE) == static_cast<int>(BringAutoDaemon::CarStatus::DRIVE));
        static_assert(static_cast<int>(State::IN_STOP) == static_cast<int>(BringAutoDaemon::CarStatus::IN_STOP));
        static_assert(static_cast<int>(Command::Action::START) == static_cast<int>(BringAutoDaemon::CarCommand::START));
        static_assert(static_cast<int>(Command::Action::STOP) == static_cast<int>(BringAutoDaemon::CarCommand::STOP));
        static_assert(static_cast<int>(Command::Action::NO_ACTION) ==
                      static_cast<int>(BringAutoDaemon::CarCommand::NO_ACTION));

    public:
        ProtoBuffer(const std::string &ipAddress, int port,
                    const std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> &context) : ipAddress_(ipAddress),
                                                                                                 port_(port),
                                                                                                 socket_(context->ioContext),
                                                                                                 context_(context) {};

        /**
         * Establishes connection with server via tcp socket (boost::asio)
         */
        void initializeConnection() override;

        /**
         * Send car status using protocol buffers and tcp to server
         */
        void sendStatus(double lat, double lon, double speed, State state, const std::string &nextStop) override;

        /**
         * Get latest received command
         * @return
         */
        const Command &getCommand() override;

        ~ProtoBuffer() override;

    private:
        boost::asio::ip::tcp::socket socket_;
        std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> context_;
        std::string ipAddress_;
        unsigned short port_;
        static const uint32_t headerSize_{4};
        using connection_buffer_t = std::array<uint8_t, 1024>;

        struct ReceivedData {
            /// buffer for receive handler
            connection_buffer_t buffer{0};
            /// Complete size of received
            uint32_t commandMessageSize{0};
        } receivedData_;

        void asyncReceiveHandler(const boost::system::error_code &error,
                                 std::size_t bytesTransferred);

        void addAsyncReceive();

        void processBufferData();
    };
}
