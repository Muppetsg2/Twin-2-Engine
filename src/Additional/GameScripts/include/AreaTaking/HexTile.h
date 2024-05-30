#pragma once

#include <core/Component.h>

#include <Generation/MapHexTile.h>

#include <Playable.h>

class HexTile : public Twin2Engine::Core::Component
{
	Generation::MapHexTile* _mapHexTile = nullptr;

public:

	bool IsFighting = false;
	Playable* takenEntity = nullptr;
	float percentage = 0.0f;
	float currCooldown = 0.0f;


	virtual void Initialize() override;
	virtual void OnDestroy() override;


	Generation::MapHexTile* GetMapHexTile() const;

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
	virtual bool DrawInheritedFields() override;
public:
	virtual void DrawEditor() override;
#endif
};