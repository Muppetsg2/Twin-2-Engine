#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class EnemyAI;

class FightingState : public State<EnemyAI> {
private:
	static DecisionTree<EnemyAI, bool> _decisionTree;

public:
	void Enter(EnemyAI* enemy) override;
	void Update(EnemyAI* enemy) override;
	void Exit(EnemyAI* enemy) override;
};