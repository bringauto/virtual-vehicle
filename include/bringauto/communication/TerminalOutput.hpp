#pragma once

#include <bringauto/communication/ICommunication.hpp>

namespace bringauto::communication {
	class TerminalOutput : public ICommunication {
	public:
		void initializeConnection() override;

		/**
		 * Print status to standard output
		 * @param lon longitude
		 * @param lat latitude
		 * @param speed actual vehicle speed
		 * @param state vehicle state
		 * @param nextStop next planned stop
		 */
		void updateStatus(double lon, double lat, double speed, State state, const std::string &nextStop) override;

		~TerminalOutput() override;
	};
}