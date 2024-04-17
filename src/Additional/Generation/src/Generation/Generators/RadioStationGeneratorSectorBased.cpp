#include <Generation/Generators/RadioStationGeneratorSectorBased.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

void RadioStationGeneratorSectorBased::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    std::vector<MapSector*> sectors;
    for (MapSector* region : tilemap->GetGameObject()->GetComponentsInChildren<MapSector>())
    {
        if (region->type == MapRegion::RegionType::Normal)
        {
            sectors.push_back(region);
        }
    }

    VectorShuffler::Shuffle(sectors);

    int sectorCountWithStation = static_cast<int>(densityFactorPerSector * sectors.size() + 0.5f);
    std::vector<MapSector*> sectorsWithStation(sectorCountWithStation);
    int instantiatedCounter = 0;

    for (int i = 0; i < sectors.size() && instantiatedCounter < sectorCountWithStation; i++)
    {
        bool guard = true;
        for (int j = 0; j < instantiatedCounter; j++)
        {
            if (sectorsWithStation[j]->HasAdjacentSector(sectors[i]))
            {
                guard = false;
                break;
            }
        }

        if (guard)
        {
            std::vector<GameObject*> foundOnes;
            for (MapHexTile* hexTiles : sectors[i]->GetTiles())
            {
                if (hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::Empty)
                {
                    foundOnes.push_back(hexTiles->GetGameObject());
                }
            }
            

            if (!foundOnes.empty())
            {
                sectorsWithStation[instantiatedCounter] = sectors[i];
                instantiatedCounter++;

                size_t randomIndex = Random::Range(0ull, foundOnes.size() - 1);
                GameObject* tile = foundOnes[randomIndex];
                tile->GetComponent<MapHexTile>()->type = MapHexTile::HexTileType::RadioStation;
                GameObject* instantiated = GameObject::Instantiate(prefabRadioStation, tile->GetTransform());
            }
        }
    }
}