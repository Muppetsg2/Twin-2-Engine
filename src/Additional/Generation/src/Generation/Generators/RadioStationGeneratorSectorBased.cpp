#include <Generation/Generators/RadioStationGeneratorSectorBased.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(RadioStationGeneratorSectorBased, Generation::Generators, "RadioStationGeneratorSectorBased")

SO_SERIALIZATION_BEGIN(RadioStationGeneratorSectorBased, AMapElementGenerator)
SO_SERIALIZE_FIELD_F(prefabRadioStation, PrefabManager::GetPrefabPath)
SO_SERIALIZE_FIELD(densityFactorPerSector)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(RadioStationGeneratorSectorBased, AMapElementGenerator)
SO_DESERIALIZE_FIELD_F_T(prefabRadioStation, PrefabManager::LoadPrefab, string)
SO_DESERIALIZE_FIELD(densityFactorPerSector)
SO_DESERIALIZATION_END()



void RadioStationGeneratorSectorBased::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    vector<MapSector*> sectors;
    hash<std::string> hasher = hash<std::string>();
    for (MapSector* region : tilemap->GetGameObject()->GetComponentsInChildren<MapSector>())
    {   
        if (hasher(MapSector::to_string(region->type)) == hasher(MapRegion::to_string(MapRegion::RegionType::Normal)))
        {
            sectors.push_back(region);
        }
    }

    VectorShuffler::Shuffle(sectors);

    int sectorCountWithStation = static_cast<int>(densityFactorPerSector * sectors.size() + 0.5f);
    vector<MapSector*> sectorsWithStation(sectorCountWithStation);
    int instantiatedCounter = 0;

    for (int i = 0; i < sectors.size() && instantiatedCounter < sectorCountWithStation; i++)
    {
        bool guard = true;
        //for (int j = 0; j < instantiatedCounter; j++)
        //{
        //    if (sectorsWithStation[j]->HasAdjacentSector(sectors[i]))
        //    {
        //        guard = false;
        //        break;
        //    }
        //}

        if (guard)
        {
            vector<GameObject*> foundOnes;
            for (MapHexTile* hexTiles : sectors[i]->GetTiles())
            {
                if (hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::Empty ||
                    hexTiles->GetGameObject()->GetComponent<MapHexTile>()->type == MapHexTile::HexTileType::None)
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
                GameObject* instantiated = SceneManager::CreateGameObject(prefabRadioStation, tile->GetTransform());
                instantiated->SetIsStatic(true);
                //GameObject* instantiated = GameObject::Instantiate(prefabRadioStation, tile->GetTransform());
            }
        }
    }
}