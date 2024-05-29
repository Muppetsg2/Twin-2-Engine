#pragma once 

#include <Playable.h>
#include <manager/SceneManager.h>

// TILEMAP
#include <Tilemap/HexagonalTilemap.h>
#include <AreaTaking/HexTile.h>

// CORE
#include <core/Random.h>

class HexTile;
class EnemyMovement;

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

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};