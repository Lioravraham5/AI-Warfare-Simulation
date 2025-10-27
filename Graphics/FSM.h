#pragma once

// FSM.h:

template <typename StateEnum>
class FSM
{
private:
	StateEnum currentState;
public: 
	FSM(StateEnum initial) { currentState = initial; }
	void setCurrentState(StateEnum s) { currentState = s; }
	StateEnum getCurrentState() const { return currentState; }
	bool isInState(StateEnum s) const { return currentState == s; }
};

