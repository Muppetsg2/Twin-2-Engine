#pragma once

#include <StateMachine/State.h>
#include <DecisionTree/DecisionTree.h>

class Enemy;
class HexTile;

class MovingState : public State<Enemy*> {
private:
	struct AfterMoveDecisionData {
		Enemy* enemy;
		std::vector<HexTile*> InMoveRangeTiles;
		std::vector<HexTile*> LocalTiles;
		std::vector<HexTile*> LocalTakenTiles;
		float localAvg;
		bool playerOnEnemyTile;
		bool enemyIsStrongerThanPlayer;
		HexTile* playerTile;
		HexTile* RadioStationTile;
	};

	static DecisionTree<Enemy*, bool> _whileMovingDecisionTree;
	static DecisionTree<AfterMoveDecisionData&, bool> _afterMoveDecisionTree;

	static std::unordered_map<Enemy*, bool> _afterMove;

	static void DoAfterMoveDecisionTree(Enemy* enemy);

	static void StartTakingOver(Enemy* enemy);
	static void MoveToFight(Enemy* enemy, HexTile* playerTile);
	static void MoveToRadioStation(Enemy* enemy, HexTile* radioStationTile);
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