#pragma once

#include <bringauto/virtual_vehicle/GlobalContext.hpp>
#include <bringauto/communication/Status.hpp>
#include <bringauto/communication/Command.hpp>

#include <boost/asio.hpp>



namespace bringauto::communication {
/**
 * @brief Generic communication interface, all communication interfaces have to inherit from this class
 */
class ICommunication {
public:
	ICommunication(const std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> &globalContext): globalContext_(
			globalContext) {};

	virtual ~ICommunication() = default;

	/**
	 * @brief connection initialization
	 * Method for initializing connection that is used, this method have to be called before
	 * making request. After calling this method, class is connected to server
	 */
	virtual void initializeConnection() = 0;

	/**
	 * @brief request to server
	 * Method sends status and receives command, can be called only after a successful initializeConnection() call
	 * if successful, command can be obtained by getCommand method
	 * @param status status to send
	 */
	virtual void makeRequest(const Status &status) = 0;

	/**
	 * @brief Return most recent command received, command is updated after each call of @makeRequest().
	 * @return Most recent command
	 */
	Command getCommand();

protected:
	Command command_; ///most actual command
	std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> globalContext_; ///global context with settings

	/**
	 * Method sends status to desired location
	 * @param status status to send
	 */
	virtual void sendStatus(const Status &status) = 0;

	/**
	 * Method for receiving command, received command will be stored in command_ variable
	 */
	virtual void receiveCommand() = 0;
};
}