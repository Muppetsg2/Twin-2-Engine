#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Time.h>
#include <GameManager.h>

#include <core/MeshRenderer.h>
#include <graphic/Material.h>

#include <Generation/MapHexTile.h>

#include <AreaTaking/HexTileTextureData.h>
#include <AreaTaking/City.h>

#include <ParticleGenerator.h>

ENUM_CLASS_BASE_VALUE(TileState, uint8_t, NONE, 0, OCCUPIED, 1, REMOTE_OCCUPYING, 2, TAKEN, 4);

class Playable;
class City;

class HexTile : public Twin2Engine::Core::Component
{
	static float _takingStage1;
	static float _takingStage2;
	static float _takingStage3;

	Generation::MapHexTile* _mapHexTile = nullptr;
	Twin2Engine::Core::MeshRenderer* _meshRenderer = nullptr;

	std::vector<HexTile*> _adjacentTiles;

	std::vector<City*> _affectingCities;
	
	size_t _textureCityStar = 0;
	size_t _textureCityBlackStar = 0;

	void TakeOver();
	void LoseInfluence();
	void UpdateTileColor();
	void UpdateBorderColor();
	void UpdateBorders();
	void CheckRoundPattern();
	void CheckNeigbhboursRoundPattern();
public:
	HexTileTextureData* texturesData;
	ParticleGenerator* particleGenerator = nullptr;
	ParticleGenerator* pgCity = nullptr;
	ParticleGenerator* pgCityNegative = nullptr;

	bool isFighting = false;

	Playable* occupyingEntity = nullptr;
	std::vector<Playable*> remotelyOccupyingEntities;
	std::vector<float> remoteMultipliers;
	float remoteMultiplier = 1.0f;
	Playable* ownerEntity = nullptr;
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
	/*Twin2Engine::Core::GameObject* TopRightBorder;
	Twin2Engine::Core::GameObject* MiddleRightBorder;
	Twin2Engine::Core::GameObject* BottomRightBorder;
	Twin2Engine::Core::GameObject* BottomLeftBorder;
	Twin2Engine::Core::GameObject* MiddleLeftBorder;
	Twin2Engine::Core::GameObject* TopLeftBorder;*/

	// Borders Joints
	std::vector<Twin2Engine::Core::GameObject*> borderJoints;
	/*Twin2Engine::Core::GameObject* TopRightLeftBorderJoint;
	Twin2Engine::Core::GameObject* TopRightRightBorderJoint;
	Twin2Engine::Core::GameObject* MiddleRightLeftBorderJoint;
	Twin2Engine::Core::GameObject* MiddleRightRightBorderJoint;
	Twin2Engine::Core::GameObject* BottomRightLeftBorderJoint;
	Twin2Engine::Core::GameObject* BottomRightRightBorderJoint;
	Twin2Engine::Core::GameObject* BottomLeftLeftBorderJoint;
	Twin2Engine::Core::GameObject* BottomLeftRightBorderJoint;
	Twin2Engine::Core::GameObject* MiddleLeftLeftBorderJoint;
	Twin2Engine::Core::GameObject* MiddleLeftRightBorderJoint;
	Twin2Engine::Core::GameObject* TopLeftLeftBorderJoint;
	Twin2Engine::Core::GameObject* TopLeftRightBorderJoint;*/

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
	void InitializeAdjacentTiles();

	void ResetTile();
	void ResetTile(Playable* n);
	void SetOwnerEntity(Playable* newOwnerEntity);
	void SetOutlineActive(bool active);
	void StartMinigame();
	void WinMinigame();
	void BadNote();
	void StartTakingOver(Playable* entity);
	void StopTakingOver(Playable* entity);
	void StartRemotelyTakingOver(Playable* entity, float multiplier = 1.0f);
	void StopRemotelyTakingOver(Playable* entity);
	int GetStage() const;

	Generation::MapHexTile* GetMapHexTile() const;

	void EnableAffected();
	void DisableAffected();
	void EnableAlbumAffected();
	void DisableAlbumAffected();

	void AddAffectingCity(City* city);
	void RemoveAffectingCity(City* city);

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};