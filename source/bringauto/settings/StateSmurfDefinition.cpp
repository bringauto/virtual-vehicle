#include <bringauto/settings/StateSmurfDefinition.hpp>



namespace bringauto::settings {
#ifdef STATE_SMURF

state_smurf::diagram::StateDiagram StateSmurfDefinition::createStateDiagram() {
	state_smurf::diagram::StateDiagram stateDiagram;
	/// Creating Vertexes, they are accessible by name.
	auto connected = stateDiagram.addVertex(connectedState);
	auto carStatusSent = stateDiagram.addVertex(statusSentState);
	auto carCommandReceived = stateDiagram.addVertex(commandReceivedState);
	auto startReceived = stateDiagram.addVertex(startReceivedState);
	auto stopReceived = stateDiagram.addVertex(stopReceivedState);
	auto idle = stateDiagram.addVertex(idleState);
	auto drive = stateDiagram.addVertex(driveState);
	auto inStop = stateDiagram.addVertex(inStopState);

	/// Setting Edges, from each state to every state it can transition
	stateDiagram.setEdge(connected, carStatusSent);

	stateDiagram.setEdge(carStatusSent, carCommandReceived);
	stateDiagram.setEdge(carCommandReceived, carStatusSent);
	stateDiagram.setEdge(carCommandReceived, stopReceived);
	stateDiagram.setEdge(carCommandReceived, startReceived);

	stateDiagram.setEdge(stopReceived, idle);
	stateDiagram.setEdge(startReceived, idle);
	stateDiagram.setEdge(startReceived, drive);
	stateDiagram.setEdge(startReceived, inStop);

	stateDiagram.setEdge(idle, connected);
	stateDiagram.setEdge(idle, carStatusSent);
	stateDiagram.setEdge(drive, carStatusSent);
	stateDiagram.setEdge(drive, inStop);
	stateDiagram.setEdge(inStop, carStatusSent);

	stateDiagram.setStartVertex(idle);

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

template<>
void StateSmurfDefinition::changeToState(const std::shared_ptr<state_smurf::transition::StateTransition>& smurf, bringauto::communication::EAutonomyAction value) {
	switch(value) {
		case communication::EAutonomyAction::E_NO_ACTION:
			break;
		case communication::EAutonomyAction::E_STOP:
			smurf->goToState(bringauto::settings::stopReceivedState);
			break;
		case communication::EAutonomyAction::E_START:
			smurf->goToState(bringauto::settings::startReceivedState);
			break;
	}
}

#endif
}
