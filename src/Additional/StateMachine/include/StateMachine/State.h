#pragma once

class StateMachine;

template<class T>
class State {
private:
	StateMachine* _parent = nullptr;

	State(const StateMachine* parent);
public:
	virtual void Enter(T* entity) = 0;
	virtual void Update(T* entity) = 0;
	virtual void Exit(T* entity) = 0;

	friend class StateMachine;
};

template<class T>
inline State<T>::State(const StateMachine* parent) : _parent(parent) {}