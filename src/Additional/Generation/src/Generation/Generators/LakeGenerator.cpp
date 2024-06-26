#include <Generation/Generators/LakeGenerator.h>

#include <manager/SceneManager.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(LakeGenerator, Generation::Generators, "LakeGenerator")

SO_SERIALIZATION_BEGIN(LakeGenerator, AMapElementGenerator)
SO_SERIALIZE_FIELD(numberOfLakes)
SO_SERIALIZE_FIELD(waterLevel)
SO_SERIALIZE_FIELD(destroyWaterTile)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(LakeGenerator, AMapElementGenerator)
SO_DESERIALIZE_FIELD(numberOfLakes)
SO_DESERIALIZE_FIELD(waterLevel)
SO_DESERIALIZE_FIELD(destroyWaterTile)
SO_DESERIALIZATION_END()


void LakeGenerator::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    list<MapRegion*> regionsList = tilemap->GetGameObject()->GetComponentsInChildren<MapRegion>();
    regionsList.remove_if([](MapRegion* region) { return region->GetTilesCount() == 0; });
    vector<MapRegion*> regions = vector<MapRegion*>(regionsList.begin(), regionsList.end());
    regionsList.clear();

    VectorShuffler::Shuffle(regions);

    bool hasNeighbour = false;

    int usedNumberOfLakes = std::min(numberOfLakes, static_cast<int>(regions.size()));
    std::vector<MapRegion*> waterRegions;
    waterRegions.reserve(usedNumberOfLakes);

    for (int i = 0; i < usedNumberOfLakes; ++i)
    {
        for (int regionIndex = 0; regionIndex < regions.size(); ++regionIndex)
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
            region->type = MapRegion::RegionType::Water;
            for (MapSector* sector : region->GetSectors())
            {
                sector->type = MapSector::SectorType::Water;
                for (MapHexTile* tile : sector->GetTiles())
                {
                    tile->type = MapHexTile::HexTileType::Water;
                }
            }

            region->GetTransform()->SetParent(nullptr);
            //tilemap->RemoveTile();
            SceneManager::DestroyGameObject(region->GetGameObject());
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
            region->GetTransform()->Translate(vec3(0.0f, waterLevel, 0.0f));
        }
    }
}

void LakeGenerator::Clear() {

}