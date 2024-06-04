#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class Enemy;

class InitState : public State<Enemy*> {
private:
	static DecisionTree<Enemy*, bool> _decisionTree;

	static void ChooseTile(Enemy* enemy);
	static void Begin(Enemy* enemy);
public:
	void Enter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Exit(Enemy* enemy) override;
};