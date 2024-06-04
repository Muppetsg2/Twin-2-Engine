#pragma once

template<class _Entity>
class State {
public:
	State() = default;
	virtual ~State() = default;

	virtual void Enter(_Entity* entity) = 0;
	virtual void Update(_Entity* entity) = 0;
	virtual void Exit(_Entity* entity) = 0;
};