#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>


class Enemy;

class RadioStationState : public State<Enemy*> {
private:
	static DecisionTree<std::pair<Enemy*, uint32_t>, bool> _decisionTree;

	static void Score(Enemy* enemy, uint32_t score);

public:
	void Enter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Exit(Enemy* enemy) override;
};