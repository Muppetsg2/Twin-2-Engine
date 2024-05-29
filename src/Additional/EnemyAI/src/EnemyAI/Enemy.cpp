#include <EnemyAI/Enemy.h>

FightingState Enemy::_fightingState;

void Enemy::Update()
{
	_stateMachine.Update(this);
}