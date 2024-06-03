#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class Enemy;
class Playable;

class FightingState : public State<Enemy*> {
private:
	ENUM_CLASS_BASE(FightResult, uint8_t, WIN, DRAW, LOSE, WAIT)

	static DecisionTree<Enemy*, FightResult> _decisionTree;

	static float SumTiles(Playable* entity);
	static float Score(Enemy* enemy, Playable* entity);

	static void Draw(Enemy* enemy);
	static void Win(Enemy* enemy);
	static void Lose(Enemy* enemy);

public:
	void Enter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Exit(Enemy* enemy) override;
};