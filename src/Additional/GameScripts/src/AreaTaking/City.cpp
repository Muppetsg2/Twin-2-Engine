#include <AreaTaking/City.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::UI;

using namespace Tilemap;

using namespace std;
using namespace glm;



void City::Initialize()
{
	_imagePictogram = GetGameObject()->GetComponentInChildren<Image>();

	MeshRenderer* mesh = GetGameObject()->GetComponent<MeshRenderer>();

	if (mesh != nullptr && _texturesData != nullptr) {
		mesh->SetMaterial(0, _texturesData->GetMaterial(_color));
	}
}

void City::Update()
{

}

void City::OnDestroy()
{
	for (size_t index = 0ull; index < _affectedTiles.size(); ++index)
	{
		//_affectedTiles[index]->RemoveAffectingCity(this);
	}
	_affectedTiles.clear();
}

bool City::IsConcertRoadCity() const
{
	return _isConcertRoadCity;
}

void City::SetConcertRoadCity(bool isConcertRoadCity)
{
	if (isConcertRoadCity != _isConcertRoadCity)
	{
		_isConcertRoadCity = isConcertRoadCity;

		if (_isConcertRoadCity)
		{
			for (size_t index = 0ull; index < _affectedTiles.size(); ++index)
			{
				_affectedTiles[index]->RemoveAffectingCity(this);
			}
			_affectedTiles.clear();
			_imagePictogram->GetTransform()->GetParent()->GetGameObject()->SetActive(true);
			_imagePictogram->SetSprite(_concertRoadCityPictogramSpriteId);
		}
		else
		{
			StartAffectingTiles(_occupiedHexTile);
			_imagePictogram->GetTransform()->GetParent()->GetGameObject()->SetActive(false);
			_imagePictogram->SetSprite(_cityPictogramSpriteId);
		}
	}
}

TILE_COLOR City::GetColor()
{
	return _color;
}

void City::SetColor(TILE_COLOR color)
{
	if (_color != color) {
		_color = color;
		if (_texturesData != nullptr) GetGameObject()->GetComponent<MeshRenderer>()->SetMaterial(0, _texturesData->GetMaterial(_color));
	}
}

float City::CalculateLooseInterestMultiplier(HexTile* hexTile)
{
	if (_occupiedHexTile->ownerEntity)
	{
		if (_occupiedHexTile->ownerEntity == hexTile->ownerEntity)
		{
			return 0.80f;
		}

		return 1.2f;
	}

	return 1.0f;
}

float City::CalculateTakingOverSpeedMultiplier(HexTile* hexTile)
{
	if (_occupiedHexTile->ownerEntity)
	{
		if (_occupiedHexTile->ownerEntity == hexTile->ownerEntity)
		{
			return 1.2f;
		}

		return 0.84f;
	}

	return 1.0f;
}

void City::StartAffectingTiles(HexTile* hexTile)
{
	_occupiedHexTile = hexTile;

	unordered_set<HexagonalTile*> affectedTiles;
	unordered_set<HexagonalTile*> tempAffectedTiles;
	unordered_set<HexagonalTile*> processedTiles;

	affectedTiles.insert(_occupiedHexTile->GetMapHexTile()->tile);
	processedTiles.insert(_occupiedHexTile->GetMapHexTile()->tile);

	HexagonalTile* adjacentTiles[6];


	for (size_t index = 0ull; index < _radius; ++index)
	{
		for (HexagonalTile* tile : processedTiles)
		{
			tile->GetAdjacentTiles(adjacentTiles);

			for (size_t i = 0ull; i < 6ull; ++i)
			{
				if (adjacentTiles[i] && adjacentTiles[i]->GetGameObject() != nullptr && !affectedTiles.contains(adjacentTiles[i]) && !tempAffectedTiles.contains(adjacentTiles[i]))
				{
					MapHexTile* mapHexTile = adjacentTiles[i]->GetGameObject()->GetComponent<MapHexTile>();

					if (mapHexTile->type != MapHexTile::HexTileType::Water && mapHexTile->type != MapHexTile::HexTileType::Mountain)
					{
						tempAffectedTiles.insert(adjacentTiles[i]);
					}
				}
			}
		}

		processedTiles.clear();
		processedTiles.insert(tempAffectedTiles.begin(), tempAffectedTiles.end());
		affectedTiles.insert(tempAffectedTiles.begin(), tempAffectedTiles.end());
		SPDLOG_INFO("CITY temp: {}", tempAffectedTiles.size());
		tempAffectedTiles.clear();
	}

	processedTiles.clear();

	_affectedTiles.clear();
	_affectedTiles.reserve(affectedTiles.size());

	HexTile* tileHexTile = nullptr;
	for (HexagonalTile* tile : affectedTiles)
	{
		tileHexTile = tile->GetGameObject()->GetComponent<HexTile>();
		tileHexTile->AddAffectingCity(this);
		_affectedTiles.push_back(tileHexTile);
	}

	affectedTiles.clear();
}


YAML::Node City::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["type"] = "City";
	node["cityPictogramSpriteId"] = SceneManager::GetSpriteSaveIdx(_cityPictogramSpriteId);
	node["concertRoadCityPictogramSpriteId"] = SceneManager::GetSpriteSaveIdx(_concertRoadCityPictogramSpriteId);
	node["radius"] = _radius;
	if (_texturesData != nullptr) {
		node["textuesData"] = Twin2Engine::Manager::ScriptableObjectManager::GetPath(_texturesData->GetId());
	}
	else {
		node["textuesData"] = "";
	}

	return node;
}

bool City::Deserialize(const YAML::Node& node)
{
	if (!Component::Deserialize(node)) return false;

	_cityPictogramSpriteId = SceneManager::GetSprite(node["cityPictogramSpriteId"].as<size_t>());
	_concertRoadCityPictogramSpriteId = SceneManager::GetSprite(node["concertRoadCityPictogramSpriteId"].as<size_t>());
	_radius = node["radius"].as<size_t>();

	string _texturesDataPath = node["textuesData"].as<string>();
	_texturesData = dynamic_cast<CityTextureData*>(Twin2Engine::Manager::ScriptableObjectManager::Load(_texturesDataPath));

	return true;
}

#if _DEBUG

void City::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("City##Component").append(id);

	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;

		int radius = _radius;
		ImGui::InputInt("Radius", &radius);
		_radius = radius;
	}
}

#endif