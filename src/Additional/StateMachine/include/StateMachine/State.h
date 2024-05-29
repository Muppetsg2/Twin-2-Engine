#pragma once

template<class T>
class State {
public:
	virtual ~State() = default;

	virtual void Enter(T* entity) = 0;
	virtual void Update(T* entity) = 0;
	virtual void Exit(T* entity) = 0;
};