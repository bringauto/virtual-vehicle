#pragma once

#include <bringauto/virtual_vehicle/GlobalContext.hpp>
#include <bringauto/communication/Status.hpp>
#include <bringauto/communication/Command.hpp>


namespace bringauto::communication {
/**
 * @brief Generic communication interface, all communication interfaces have to inherit from this class
 */
class ICommunication {
public:
	explicit ICommunication(const std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> &globalContext)
			: globalContext_(
			globalContext) {};

	/**
	 * @brief connection initialization
	 * Method for initializing connection that is used, this method have to be called before
	 * making request. After calling this method, class is connected to server
	 * @return true if connection was successful, false otherwise
	 */
	virtual bool initializeConnection() = 0;

	/**
	 * @brief request to server
	 * Method sends status and receives command, can be called only after a successful initializeConnection() call
	 * if successful, command can be obtained by getCommand method
	 * @param status status to send
	 * @return true if request was successful, false otherwise
	 */
	virtual bool makeRequest(const Status &status) = 0;

	/**
	 * @brief Return most recent command received, command is updated after each call of @makeRequest().
	 * @return Most recent command
	 */
	Command getCommand();

	/**
	 * @brief Check if connection is alive
	 * @return true if connection is alive, false otherwise
	 */
	[[nodiscard]] bool isConnected() const;

	virtual ~ICommunication() = default;

protected:
	/// Current command
	Command currentCommand_;
	/// Global context with settings
	std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> globalContext_;
	/// Indication if connection is alive
	bool isConnected_ { false };
};
}