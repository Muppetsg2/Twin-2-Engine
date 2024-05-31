#include <EnemyAI/EnemyAI.h>

FightingState EnemyAI::_fightingState;

void EnemyAI::Initialize()
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	_stateMachine.ChangeState(this, &_fightingState);
}

void EnemyAI::Update()
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	_stateMachine.Update(this);
}