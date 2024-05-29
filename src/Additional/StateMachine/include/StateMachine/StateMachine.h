#pragma once

#include <StateMachine/State.h>

template<class T>
class StateMachine {
private:
	State<T>* _currentState = nullptr;

public:
	void Update(T* entity);
	void ChangeState(T* entity, State<T>* newState);
};

template<class T>
void StateMachine<T>::Update(T* entity) {
	if (_currentState == nullptr) return;
	_currentState->Update(entity);
}

template<class T>
void StateMachine<T>::ChangeState(T* entity, State<T>* newState) {
	if (_currentState != nullptr) _currentState->Exit(entity);
	_currentState = newState;
	_currentState->Enter(entity);
}