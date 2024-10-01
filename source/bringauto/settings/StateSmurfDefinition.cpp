#include <bringauto/settings/StateSmurfDefinition.hpp>



namespace bringauto::settings {
#ifdef STATE_SMURF

state_smurf::diagram::StateDiagram StateSmurfDefinition::createStateDiagram() {
	state_smurf::diagram::StateDiagram stateDiagram;
	/// Creating Vertexes, they are accessible by name.
	auto connected = stateDiagram.addVertex(connectedState);
	auto idle = stateDiagram.addVertex(idleState);
	auto drive = stateDiagram.addVertex(driveState);
	auto inStop = stateDiagram.addVertex(inStopState);

	/// Setting Edges, from each state to every state it can transition
	stateDiagram.setEdge(connected, drive);

	stateDiagram.setEdge(drive, inStop);
	stateDiagram.setEdge(drive, idle);
	stateDiagram.setEdge(inStop, drive);
	stateDiagram.setEdge(inStop, idle);
	stateDiagram.setEdge(idle, drive);
	stateDiagram.setEdge(idle, inStop);

	stateDiagram.setStartVertex(connected);

	return stateDiagram;
}

template<>
void StateSmurfDefinition::changeToState(const std::shared_ptr<state_smurf::transition::StateTransition>& smurf, bringauto::communication::EAutonomyState value) {
	switch(value) {
		case communication::EAutonomyState::E_IDLE:
			smurf->goToState(bringauto::settings::idleState);
			break;
		case communication::EAutonomyState::E_DRIVE:
			smurf->goToState(bringauto::settings::driveState);
			break;
		case communication::EAutonomyState::E_IN_STOP:
			smurf->goToState(bringauto::settings::inStopState);
			break;
		case communication::EAutonomyState::E_OBSTACLE:
			break;
		case communication::EAutonomyState::E_ERROR:
			break;
	}

}

#endif
}
