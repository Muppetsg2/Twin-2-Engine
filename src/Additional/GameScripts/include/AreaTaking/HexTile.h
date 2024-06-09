#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Time.h>
#include <GameManager.h>

#include <core/MeshRenderer.h>
#include <graphic/Material.h>

#include <Generation/MapHexTile.h>

#include <AreaTaking/HexTileTexturesData.h>


ENUM_CLASS_BASE_VALUE(TileState, uint8_t, NONE, 0, OCCUPIED, 1, REMOTE_OCCUPYING, 2, TAKEN, 4);

class Playable;

class HexTile : public Twin2Engine::Core::Component
{
	static float _takingStage1;
	static float _takingStage2;
	static float _takingStage3;

	Generation::MapHexTile* _mapHexTile = nullptr;
	Twin2Engine::Core::MeshRenderer* _meshRenderer = nullptr;

	void TakeOver();
	void LoseInfluence();
	void UpdateTileColor();
	void UpdateBorders();
	void CheckRoundPattern();
public:
	HexTileTextureData* textuesData;


	bool isFighting = false;


	Playable* occupyingEntity = nullptr;
	std::vector<Playable*> remotelyOccupyingEntities;
	std::vector<float> remoteMultipliers;
	float remoteMultiplier = 1.0f;
	Playable* takenEntity = nullptr;
	float percentage = 0.0f;
	float currCooldown = 0.0f;

	glm::vec4 baseColor;
	float alphaTransparencyObject;
	TileState state;

	//float minLosePercentage;
	float loseInfluenceSpeed = 1.0f;
	//float loseInfluenceDelay;
	//float currLoseInfluenceDelay;
	
	// Borders
	std::vector<Twin2Engine::Core::GameObject*> borders;
	std::vector<Twin2Engine::Core::GameObject*> borderJoints;

	int totalTileFans;

	glm::vec3 sterowiecPos;

	float badNotePercent = 5.0f;
	bool minigameActive = false;
	bool isAlbumActive = false;

	float radioStationCooldown = 3.0f;
	//Material* tileMaterial;
	//Outline* outline;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	void ResetTile();
	void SetOutlineActive(bool active);
	void StartMinigame();
	void WinMinigame();
	void BadNote();
	void StartTakingOver(Playable* entity);
	void StopTakingOver(Playable* entity);
	void StartRemotelyTakingOver(Playable* entity, float multiplier = 1.0f);
	void StopRemotelyTakingOver(Playable* entity);

	Generation::MapHexTile* GetMapHexTile() const;

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};