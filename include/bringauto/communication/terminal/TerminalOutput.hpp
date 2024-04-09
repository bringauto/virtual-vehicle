#pragma once

#include <bringauto/communication/ICommunication.hpp>



namespace bringauto::communication::terminal {
/**
 * @brief Class for no fleet connection
 */
class TerminalOutput final: public ICommunication {
public:
	explicit TerminalOutput(const std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> &globalContext)
			: ICommunication(
			globalContext) {};

	/**
	 * @brief Initialize fake connection, always returns true
	 * @return true
	 */
	bool initializeConnection() override;

	/**
	 * @brief Print status to terminal, receive fake empty command
	 * @param status status to be printed
	 * @return true
	 */
	bool makeRequest(const Status &status) override;

	~TerminalOutput() override = default;
};
}