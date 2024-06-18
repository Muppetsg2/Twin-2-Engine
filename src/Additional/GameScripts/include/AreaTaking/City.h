#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>

#include <tilemap/HexagonalTile.h>

#include <AreaTaking/HexTile.h>

class HexTile;

class City : public Twin2Engine::Core::Component
{
	HexTile* _occupiedHexTile;


public:

	float CalculateLooseInterestMultiplier(HexTile* hexTile);
	float CalculateTakingOverSpeedMultiplier(HexTile* hexTile);

	void StartAffectingTiles(HexTile* hexTile);

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};