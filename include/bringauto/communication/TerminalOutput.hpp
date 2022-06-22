#pragma once

#include <bringauto/communication/ICommunication.hpp>



namespace bringauto::communication {
/**
 * @brief Class for no fleet connection
 */
class TerminalOutput final: public ICommunication {
public:
	TerminalOutput(const std::shared_ptr<bringauto::virtual_vehicle::GlobalContext> &globalContext): ICommunication(
			globalContext) {};

	void initializeConnection() override;

	void makeRequest(const Status &status) override;

	~TerminalOutput() override;

private:
	void sendStatus(const Status &status) override;

	void receiveCommand() override;
};
}