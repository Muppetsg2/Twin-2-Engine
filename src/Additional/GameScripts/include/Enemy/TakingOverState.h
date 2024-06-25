#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class Enemy;

class TakingOverState : public State<Enemy*> {
private:
	static DecisionTree<Enemy*, bool> _decisionTree;

	static void AlbumAbility(Enemy* enemy);
	static void FansMeetingAbility(Enemy* enemy);
	static void Move(Enemy* enemy);

public:
	void Enter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Exit(Enemy* enemy) override;
};