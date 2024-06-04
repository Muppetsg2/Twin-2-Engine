#pragma once 

#include <manager/SceneManager.h>

// TILEMAP
#include <Tilemap/HexagonalTilemap.h>
#include <AreaTaking/HexTile.h>

// CORE
#include <core/Random.h>

#include <Playable.h>

// STATES
#include <StateMachine/StateMachine.h>
#include <Enemy/FightingState.h>
#include <Enemy/MovingState.h>
#include <Enemy/RadioStationState.h>
#include <Enemy/TakingOverState.h>

class HexTile;
class EnemyMovement;

using namespace Twin2Engine::Core;

class Enemy : public Playable {
	Tilemap::HexagonalTilemap* _tilemap = nullptr;
	EnemyMovement* _movement = nullptr;

	// GENERATIVE PARAMETERS
	float _noteLuck = 50.f;
	float _winChance = 33.33f;
	float _drawChance = 33.33f;

	// STATE MACHINE
	StateMachine<Enemy*> _stateMachine;

	static TakingOverState _takingOverState;
	static MovingState _movingState;
	static FightingState _fightingState;
	static RadioStationState _radioStationState;

	friend class TakingOverState;
	friend class MovingState;
	friend class FightingState;
	friend class RadioStationState;

	void ChangeState(State<Enemy*>* newState);

public:

	//Tymczasowe
	bool isTakingArea = false;
	float takingAreaCounter = 0.0f;
	float targetPercentage = 90.0f;


	//int colorIdx = 0;
	float TakeOverSpeed = 1.0f;

	float albumTime = 1.0f;
	float albumCooldown = 1.0f;
	float fansTime = 1.0f;
	float fansCooldown = 1.0f;
	float radioTime = 1.0f;

	float paperRockScisorsWinLuck  = 1.0f;
	float paperRockScisorsDrawLuck = 1.0f;

	float radioWinNoteLuck = 1.0f;
	float albumUseLuck = 1.0f;
	float concertUseLuck = 1.0f;
	float fansUseLuck = 1.0f;

	float functionData = 1.0f;
	float upgradeChance = 1.0f;

	virtual void Initialize() override;
	virtual void OnEnable() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	void FinishedMovement(HexTile* hexTile);

	void PerformMovement();

	virtual void LostPaperRockScissors(Playable* playable) override;
	virtual void WonPaperRockScissors(Playable* playable) override;
	virtual void LostFansControl(Playable* playable) override;
	virtual void WonFansControl(Playable* playable) override;
	virtual void StartPaperRockScissors(Playable* playable) override;
	virtual void StartFansControl(Playable* playable) override;

	float GetMaxRadius() const override;

protected:
	virtual void OnDead() override;

public:

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};