#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>

#include <ui/Image.h>

#include <tilemap/HexagonalTile.h>

#include <AreaTaking/HexTile.h>

#include <AreaTaking/CityTextureData.h>

class HexTile;

class City : public Twin2Engine::Core::Component
{
	HexTile* _occupiedHexTile = nullptr;
	Twin2Engine::UI::Image* _imagePictogram = nullptr;

	size_t _cityPictogramSpriteId;
	size_t _concertRoadCityPictogramSpriteId;
	size_t _radius = 1ull;

	std::vector<HexTile*> _affectedTiles;

	bool _isConcertRoadCity = false;

	CityTextureData* _texturesData;
	TILE_COLOR _color = TILE_COLOR::NEUTRAL;

public:

	float CalculateLooseInterestMultiplier(HexTile* hexTile);
	float CalculateTakingOverSpeedMultiplier(HexTile* hexTile);

	void StartAffectingTiles(HexTile* hexTile);

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	bool IsConcertRoadCity() const;
	void SetConcertRoadCity(bool isConcertRoadCity);

	TILE_COLOR GetColor();
	void SetColor(TILE_COLOR color);

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};