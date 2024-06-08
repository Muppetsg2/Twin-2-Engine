#include <Generation/Generators/RadioStationGeneratorRegionBased.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(RadioStationGeneratorRegionBased, Generation::Generators, "RadioStationGeneratorRegionBased")

SO_SERIALIZATION_BEGIN(RadioStationGeneratorRegionBased, AMapElementGenerator)
SO_SERIALIZE_FIELD_F(prefabRadioStation, PrefabManager::GetPrefabPath)
SO_SERIALIZE_FIELD(densityFactorPerRegion)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(RadioStationGeneratorRegionBased, AMapElementGenerator)
SO_DESERIALIZE_FIELD_F_T(prefabRadioStation, PrefabManager::LoadPrefab, string)
SO_DESERIALIZE_FIELD(densityFactorPerRegion)
SO_DESERIALIZATION_END()

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

    for (int i = 0; i < regions.size() && instantiatedCounter < regionCountWithStation; ++i)
    {
        bool guard = true;
        for (int j = 0; j < instantiatedCounter; ++j)
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
                    if (hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::Empty ||
                        hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::None)
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
                GameObject* instantiated = SceneManager::CreateGameObject(prefabRadioStation, tile->GetTransform());
                instantiated->SetIsStatic(true);
                //GameObject* instantiated = GameObject::Instantiate(prefabRadioStation, tile->GetTransform());
            }
        }
    }
}