#pragma once

#include <StateMachine/State.h>

template<class _Entity>
class StateMachine {
private:
	State<_Entity>* _currentState = nullptr;

public:
	StateMachine() = default;
	virtual ~StateMachine() = default;

	void Update(_Entity entity);
	void ChangeState(_Entity entity, State<_Entity>* newState);
};

template<class _Entity>
void StateMachine<_Entity>::Update(_Entity entity) {
	if (_currentState == nullptr) return;
	_currentState->Update(entity);
}

template<class _Entity>
void StateMachine<_Entity>::ChangeState(_Entity entity, State<_Entity>* newState) {
	if (_currentState != nullptr) _currentState->Exit(entity);
	_currentState = newState;
	_currentState->Enter(entity);
}