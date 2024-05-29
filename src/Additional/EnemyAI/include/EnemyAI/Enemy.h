#pragma once

#include <core/Component.h>
#include <StateMachine/StateMachine.h>
#include <EnemyAI/FightingState.h>

class Enemy : public Twin2Engine::Core::Component {
private:
	static FightingState _fightingState;

	StateMachine<Enemy> _stateMachine;

public:
	void Update() override;
};