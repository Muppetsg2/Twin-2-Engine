#pragma once

#include <core/Component.h>
#include <StateMachine/StateMachine.h>
#include <EnemyAI/FightingState.h>

class EnemyAI : public Twin2Engine::Core::Component {
private:
	static FightingState _fightingState;

	StateMachine<EnemyAI> _stateMachine;

public:
	void Initialize() override;
	void Update() override;
};