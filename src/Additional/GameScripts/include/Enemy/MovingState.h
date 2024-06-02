#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class Enemy;

class MovingState : public State<Enemy*> {
private:
	static DecisionTree<Enemy*, bool> _decisionTree;

	static void StartTakingOver(Enemy* enemy);
	static void Fight(Enemy* enemy);
	static void AlbumAbility(Enemy* enemy);
	static void ConcertAbility(Enemy* enemy);
	static void Move(Enemy* enemy);

public:
	void Enter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Exit(Enemy* enemy) override;
};