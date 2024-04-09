#pragma once

#include <bringauto/settings/Settings.hpp>

#include <boost/asio.hpp>

#ifdef STATE_SMURF
#include <state_smurf/transition/StateTransition.hpp>
#include <bringauto/settings/StateSmurfDefinition.hpp>
#endif


namespace bringauto::virtual_vehicle {
/**
 * @brief Class controlling global variables
 */
struct GlobalContext {
#ifdef STATE_SMURF
	std::shared_ptr<state_smurf::transition::StateTransition> transitions;
#endif
	boost::asio::io_context ioContext;
	std::shared_ptr<settings::Settings> settings;
};
}