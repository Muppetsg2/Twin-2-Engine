#include <AreaTaking/City.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Physic;
//using namespace Twin2Engine::UI;

using namespace Tilemap;

using namespace std;
using namespace glm;



void City::Initialize()
{

}

void City::Update()
{

}

void City::OnDestroy()
{

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

	processedTiles.insert(_occupiedHexTile->GetMapHexTile()->tile);

	HexagonalTile* adjacentTiles[6];

	size_t _radius = 1ull;

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
		tempAffectedTiles.clear();
	}

	for (HexagonalTile* tile : affectedTiles)
	{
		tile->GetGameObject()->GetComponent<HexTile>()->AddAffectingCity(this);
	}

	affectedTiles.clear();
}


YAML::Node City::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["type"] = "City";

	return node;
}

bool City::Deserialize(const YAML::Node& node)
{
	if (!Component::Deserialize(node)) return false;


	return true;
}

#if _DEBUG

void City::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("City##Component").append(id);

	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;

	}
}

#endif