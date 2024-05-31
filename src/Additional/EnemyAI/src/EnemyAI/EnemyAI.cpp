#include <EnemyAI/EnemyAI.h>

FightingState EnemyAI::_fightingState;

void EnemyAI::Initialize()
{
	_stateMachine.ChangeState(this, &_fightingState);
}

void EnemyAI::Update()
{
	_stateMachine.Update(this);
}