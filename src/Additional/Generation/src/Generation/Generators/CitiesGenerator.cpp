#include <Generation/Generators/CitiesGenerator.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(CitiesGenerator, Generation::Generators, "CitiesGenerator")

void CitiesGenerator::Serialize(YAML::Node& node) const
{

}
bool CitiesGenerator::Deserialize(const YAML::Node& node)
{
    return true;
}
void CitiesGenerator::Generate(HexagonalTilemap* tilemap)
{
    if (byRegions)
    {
        std::vector<MapRegion*> regions;
        for (MapRegion* region : tilemap->GetGameObject()->GetComponentsInChildren<MapRegion>())
        {
            if (region->type == MapRegion::RegionType::Normal)
            {
                regions.push_back(region);
            }
        }
        VectorShuffler::Shuffle(regions);

        for (int i = 0; i < static_cast<int>(density * regions.size()); i++)
        {
            std::vector<GameObject*> foundOnes;
            for (MapSector* sector : regions[i]->GetSectors())
            {
                for (MapHexTile* hexTiles : sector->GetTiles())
                {
                    if (hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::Empty ||
                        hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::None)
                    {
                        foundOnes.push_back(hexTiles->GetGameObject());
                    }
                }
            }

            if (!foundOnes.empty())
            {
                vec3 avgPosition;
                for (GameObject* foundOne : foundOnes)
                {
                    avgPosition += foundOne->GetTransform()->GetGlobalPosition();
                }
                avgPosition /= foundOnes.size();

                GameObject* found = foundOnes[0];
                float distance = glm::distance(found->GetTransform()->GetGlobalPosition(), avgPosition);
                for (GameObject* foundOne : foundOnes)
                {
                    float currentDistance = glm::distance(found->GetTransform()->GetGlobalPosition(), avgPosition);
                    if (currentDistance < distance)
                    {
                        found = foundOne;
                        distance = currentDistance;
                    }
                }
                found->GetComponent<MapHexTile>()->type = MapHexTile::HexTileType::PointOfInterest;
                GameObject* instantiated = GameObject::Instantiate(prefabCity, found->GetTransform());
            }
        }
    }
    else
    {
        std::vector<MapSector*> sectors;
        for (MapSector* sector : tilemap->GetGameObject()->GetComponentsInChildren<MapSector>())
        {
            if (sector->type == MapSector::SectorType::Normal)
            {
                sectors.push_back(sector);
            }
        }
        VectorShuffler::Shuffle(sectors);

        for (int i = 0; i < static_cast<int>(density * sectors.size()); i++)
        {
            std::vector<GameObject*> foundOnes;
            for (MapHexTile* tile : sectors[i]->GetTiles())
            {
                if (tile->type == MapHexTile::HexTileType::Empty ||
                    tile->type == MapHexTile::HexTileType::None)
                {
                    foundOnes.push_back(tile->GetGameObject());
                }
            }

            if (!foundOnes.empty())
            {
                vec3 avgPosition;
                for (GameObject* foundOne : foundOnes)
                {
                    avgPosition += foundOne->GetTransform()->GetGlobalPosition();
                }
                avgPosition /= foundOnes.size();

                GameObject* found = foundOnes[0];
                float distance = glm::distance(found->GetTransform()->GetGlobalPosition(), avgPosition);
                for (GameObject* foundOne : foundOnes)
                {
                    float currentDistance = glm::distance(found->GetTransform()->GetGlobalPosition(), avgPosition);
                    if (currentDistance < distance)
                    {
                        found = foundOne;
                        distance = currentDistance;
                    }
                }
                found->GetComponent<MapHexTile>()->type = MapHexTile::HexTileType::PointOfInterest;
                GameObject* instantiated = GameObject::Instantiate(prefabCity, found->GetTransform());
            }
        }
    }
}