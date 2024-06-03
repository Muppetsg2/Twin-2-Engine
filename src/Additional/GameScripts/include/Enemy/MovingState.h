#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class Enemy;

class MovingState : public State<Enemy*> {
private:
	static DecisionTree<Enemy*, bool> _decisionTree;

	static void StartTakingOver(Enemy* enemy);
	static void Fight(Enemy* enemy);
	static void RadioStation(Enemy* enemy);
	static void AlbumAbility(Enemy* enemy);
	static void ConcertAbility(Enemy* enemy);
	static void Move(Enemy* enemy);

	static void ChooseTile(Enemy* enemy);
	static std::unordered_map<Enemy*, std::pair<size_t, size_t>> _eventsIds;

public:
	void Enter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Exit(Enemy* enemy) override;
};