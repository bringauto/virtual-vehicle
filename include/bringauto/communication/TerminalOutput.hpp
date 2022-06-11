#pragma once

#include <bringauto/communication/ICommunication.hpp>



namespace bringauto::communication {
class TerminalOutput final: public ICommunication {
public:
	void initializeConnection() override;

	void makeRequest(const Status &status) override;

	~TerminalOutput() override;

private:
	void sendStatus(const Status &status) override;

	void receiveCommand() override;
};
}