#pragma once

#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <CarStateProtocol.pb.h>

#include <boost/asio.hpp>



namespace bringauto::communication {
/**
 * Specific communication interface implementation, uses proto buffer library to send and receive messages from server
 */
class ProtoBuffer final: public ICommunication {
	/**
	 * Checking if proto buffer protocol and ICommunication protocol are the same
	 */
	static_assert(static_cast<int>(Status::State::IDLE) == static_cast<int>(CarStateProtocol::CarStatus::IDLE));
	static_assert(static_cast<int>(Status::State::DRIVE) == static_cast<int>(CarStateProtocol::CarStatus::DRIVE));
	static_assert(static_cast<int>(Status::State::IN_STOP) == static_cast<int>(CarStateProtocol::CarStatus::IN_STOP));
	static_assert(static_cast<int>(Command::Action::START) == static_cast<int>(CarStateProtocol::CarCommand::START));
	static_assert(static_cast<int>(Command::Action::STOP) == static_cast<int>(CarStateProtocol::CarCommand::STOP));
	static_assert(static_cast<int>(Command::Action::NO_ACTION) ==
				  static_cast<int>(CarStateProtocol::CarCommand::NO_ACTION));

public:
	ProtoBuffer(const std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> &globalContext): ICommunication(
			globalContext), socket_(globalContext->ioContext) {};

	void makeRequest(const Status &status) override;

	void initializeConnection() override;

	~ProtoBuffer() override;

private:
	boost::asio::ip::tcp::socket socket_;
	static const uint32_t headerSize_ { 4 };
	using connection_buffer_t = std::array<uint8_t, 1024>;

	struct ReceivedData {
		/// buffer for receive handler
		connection_buffer_t buffer { 0 };
		/// Complete size of received
		uint32_t commandMessageSize { 0 };
	} receivedData_;

	void receiveCommand() override;

	void sendStatus(const Status &status) override;

	void processBufferData();

	std::string generateCarStatusString(const Status &status);


};
}
