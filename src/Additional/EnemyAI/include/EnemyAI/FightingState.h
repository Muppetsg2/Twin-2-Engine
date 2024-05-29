#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class Enemy;

class FightingState : public State<Enemy> {
private:
	static DecisionTree<Enemy, bool> _decisionTree;

public:
	void Enter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Exit(Enemy* enemy) override;
};