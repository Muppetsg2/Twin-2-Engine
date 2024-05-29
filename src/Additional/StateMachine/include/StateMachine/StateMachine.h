#pragma once

#include <StateMachine/State.h>

template<class T>
class StateMachine {
private:
	State<T>* _currentState = nullptr;

public:
	void Update(T* entity);
	void ChangeState(State<T>* newState);
};