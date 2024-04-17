#include <Generation/Generators/LakeGenerator.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

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
            //DestroyImmediate(region->GetGameObject());
        }
    }
    else
    {
        for (MapRegion* region : waterRegions)
        {
            region->type = MapRegion::RegionType::Water;
            for (MapSector* sector : region->regionSectors)
            {
                sector->type = MapSector::SectorType::Water;
                for (GameObject* tile : sector->sectorTiles)
                {
                    tile->GetComponent<HexTile>()->Type = TileType::Water;
                    tile->GetComponent<HexTile>()->baseColor = waterColor;
                }
            }
            region->transform->SetPositionY(waterLevel);
        }
    }
}