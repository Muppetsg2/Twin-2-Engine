#include <Generation/Generators/LakeGenerator.h>

#include <manager/SceneManager.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

void LakeGenerator::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    list<MapRegion*> regionsList = tilemap->GetGameObject()->GetComponentsInChildren<MapRegion>();
    std::vector<MapRegion*> regions = vector<MapRegion*>(regionsList.begin(), regionsList.end());
    regions.clear();

    VectorShuffler::Shuffle(regions);

    bool hasNeighbour = false;

    int usedNumberOfLakes = std::min(numberOfLakes, static_cast<int>(regions.size()));
    std::vector<MapRegion*> waterRegions;
    waterRegions.reserve(usedNumberOfLakes);

    for (int i = 0; i < usedNumberOfLakes; i++)
    {
        for (int regionIndex = 0; regionIndex < regions.size(); regionIndex++)
        {
            hasNeighbour = false;
            for (MapRegion* region : waterRegions)
            {
                if (regions[regionIndex] == region || regions[regionIndex]->HasAdjacentRegion(region))
                {
                    hasNeighbour = true;
                    break;
                }
            }

            if (!hasNeighbour)
            {
                waterRegions.push_back(regions[regionIndex]);
                break;
            }
        }
    }

    if (destroyWaterTile)
    {
        for (MapRegion* region : waterRegions)
        {
            SPDLOG_ERROR("Nie zrobiono usuwania GameObjectów");
            //SceneManager::DeleteGameObject(region->GetGameObject());
            //DestroyImmediate(region->GetGameObject());
        }
    }
    else
    {
        for (MapRegion* region : waterRegions)
        {
            region->type = MapRegion::RegionType::Water;
            for (MapSector* sector : region->GetSectors())
            {
                sector->type = MapSector::SectorType::Water;
                for (MapHexTile* tile : sector->GetTiles())
                {
                    tile->type = MapHexTile::HexTileType::Water;
                }
            }
            region->GetGameObject()->GetTransform()->Translate(vec3(0.0f, -waterLevel, 0.0f));
        }
    }
}