#pragma once 

#include <manager/SceneManager.h>

// TILEMAP
#include <Tilemap/HexagonalTilemap.h>
#include <AreaTaking/HexTile.h>

// CORE
#include <core/Random.h>


#include <Playable.h>

class HexTile;
class EnemyMovement;
//class Playable;

using namespace Twin2Engine::Core;

class Enemy : public Playable {
	Tilemap::HexagonalTilemap* _tilemap = nullptr;
	EnemyMovement* _movement = nullptr;
	std::vector<HexTile*> _tiles;

public:
	int colorIdx = 0;
	float TakeOverSpeed = 1.0f;

	float albumTime = 1.0f;
	float albumCooldown = 1.0f;
	float fansTime = 1.0f;
	float fansCooldown = 1.0f;
	float radioTime = 1.0f;

	float paperRockScisorsWinLuck = 1.0f;
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

	void PerformMovement();

	virtual void LostPaperRockScissors(Playable* playable) override;
	virtual void WonPaperRockScissors(Playable* playable) override;
	virtual void LostFansControl(Playable* playable) override;
	virtual void WonFansControl(Playable* playable) override;
	virtual void StartPaperRockScissors(Playable* playable) override;
	virtual void StartFansControl(Playable* playable) override;

protected:
	virtual void OnDead() override;

public:

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};