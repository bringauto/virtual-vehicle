#pragma once

#ifdef STATE_SMURF

#include <bringauto/communication/Status.hpp>
#include <bringauto/communication/Command.hpp>
#include <state_smurf/diagram/StateDiagram.hpp>
#include <state_smurf/transition/StateTransition.hpp>



namespace bringauto::settings {
/// State names
const std::string connectedState { "Connected" };
const std::string idleState { "Idle" };
const std::string driveState { "Drive" };
const std::string inStopState { "InStop" };

/**
 * @brief Class working with StateSmurf testing framework. This class defines state graph and makes state transitions
 */
class StateSmurfDefinition {
public:
	/**
	 * @brief Method will create state graph defined in readme
	 * @return defined state graph
	 */
	static state_smurf::diagram::StateDiagram createStateDiagram();

	/**
	 * @brief Template method for changing state in state graph
	 * @tparam T enum type
	 * @param smurf TransitionSmurf containing graph
	 * @param value value of enum
	 */
	template <typename T>
	static void changeToState(const std::shared_ptr<state_smurf::transition::StateTransition> &smurf, T value);
};
}

#endif
