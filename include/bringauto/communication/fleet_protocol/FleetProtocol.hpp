#pragma once

#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <MissionModule.pb.h>
#include <fleet_protocol/internal_client/internal_client.h>

#include <boost/asio.hpp>

#include <chrono>



namespace bringauto::communication::fleet_protocol{
/**
 * Specific communication interface implementation, uses proto buffer library to send and receive messages from server
 */
class FleetProtocol final: public ICommunication {
public:
	explicit FleetProtocol(const std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> &globalContext): ICommunication(
			globalContext) {};

	/**
	 * @brief Method will send given status to server and wait for command to be received
	 * @param status status to be sent
	 * @return true if request was successful, false otherwise
	 */
	bool makeRequest(const Status &status) override;

	/**
	 * @brief Initialize connection to server, internal client will be created and connected to server
	 * @return true if connection was successful, false otherwise
	 */
	bool initializeConnection() override;

	/**
	 * @brief Destroy connection to server, internal client will be destroyed and disconnected from server
	 */
	~FleetProtocol() override;

private:
	/// Mission module ID
	static constexpr int MISSION_MODULE_NUMBER{1};
	/// Autonomy device ID
	static constexpr int AUTONOMY_DEVICE_NUMBER{1};
	/// Context of internalClient
	void *internalClientContext_{ nullptr };
	/// Last connection attempt
	uint64_t lastConnectAttemptS_{0};


	bool receiveCommand();

	bool sendStatus(const Status &status);

	void processBufferData(const buffer &bufferData);

	std::string generateCarStatusString(const Status &status);

};
}
