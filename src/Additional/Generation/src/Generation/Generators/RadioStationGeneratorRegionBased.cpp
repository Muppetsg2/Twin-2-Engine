#include <Generation/Generators/RadioStationGeneratorRegionBased.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

void RadioStationGeneratorRegionBased::Generate(Tilemap::HexagonalTilemap* tilemap)
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

    int regionCountWithStation = static_cast<int>(densityFactorPerRegion * regions.size() + 0.5f);
    std::vector<MapRegion*> regionWithStation(regionCountWithStation);
    int instantiatedCounter = 0;

    for (int i = 0; i < regions.size() && instantiatedCounter < regionCountWithStation; i++)
    {
        bool guard = true;
        for (int j = 0; j < instantiatedCounter; j++)
        {
            if (regionWithStation[j]->HasAdjacentRegion(regions[i]))
            {
                guard = false;
                break;
            }
        }

        if (guard)
        {
            std::vector<GameObject*> foundOnes;
            for (MapSector* sector : regions[i]->GetSectors())
            {
                for (MapHexTile* hexTiles : sector->GetTiles())
                {
                    if (hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::Empty)
                    {
                        foundOnes.push_back(hexTiles->GetGameObject());
                    }
                }
            }

            if (!foundOnes.empty())
            {
                regionWithStation[instantiatedCounter] = regions[i];
                instantiatedCounter++;

                size_t randomIndex = Random::Range(0ull, foundOnes.size() - 1);
                GameObject* tile = foundOnes[randomIndex];
                tile->GetComponent<MapHexTile>()->type = MapHexTile::HexTileType::RadioStation;
                GameObject* instantiated = GameObject::Instantiate(prefabRadioStation, tile->GetTransform());
            }
        }
    }
}