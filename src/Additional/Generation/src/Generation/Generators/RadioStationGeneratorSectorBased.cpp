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
SO_DESERIALIZE_FIELD_R("prefabRadioStation", prefabPath)
SO_DESERIALIZE_FIELD_F_T(prefabRadioStation, PrefabManager::LoadPrefab, string)
SO_DESERIALIZE_FIELD(densityFactorPerSector)
SO_DESERIALIZATION_END()



void RadioStationGeneratorSectorBased::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    if (!prefabRadioStation)
    {
        prefabRadioStation = PrefabManager::LoadPrefab(prefabPath);
    }

    vector<MapSector*> sectors;
    hash<std::string> hasher = hash<std::string>();
    for (MapSector* sector : tilemap->GetGameObject()->GetComponentsInChildren<MapSector>())
    {   
        //if (hasher(MapSector::to_string(sector->type)) == hasher(MapRegion::to_string(MapRegion::RegionType::Normal)))
        if (hasher(MapSector::to_string(sector->type)) == hasher(MapRegion::to_string(MapRegion::RegionType::Normal)) && sector->IsInternalSector())
        {
            sectors.push_back(sector);
        }
    }

    VectorShuffler::Shuffle(sectors);

    int sectorCountWithStation = static_cast<int>(densityFactorPerSector * sectors.size() + 0.5f);
    vector<MapSector*> sectorsWithStation(sectorCountWithStation);
    int instantiatedCounter = 0;

    /*
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

                GameObject* instantiated = nullptr;
                if (PrefabManager::GetPrefab(prefabRadioStation->GetId()) != nullptr) {
                    instantiated = SceneManager::CreateGameObject(prefabRadioStation, tile->GetTransform());
                }
                else {

                    if (prefabPath != "") {
                        prefabRadioStation = PrefabManager::LoadPrefab(prefabPath);
                        instantiated = SceneManager::CreateGameObject(prefabRadioStation, tile->GetTransform());
                    }
                    else {
                        instantiated = SceneManager::CreateGameObject(tile->GetTransform());
                    }
                }

                //GameObject* instantiated = SceneManager::CreateGameObject(prefabRadioStation, tile->GetTransform());
                instantiated->SetIsStatic(true);
                //GameObject* instantiated = GameObject::Instantiate(prefabRadioStation, tile->GetTransform());
            }
        }
    }
    /**/

    unordered_map<MapHexTile*, pair<unordered_map<MapHexTile*, float>, float>> connections;

    //vector<MapSector*> sectors;
    //
    //for (MapSector* sector : tilemap->GetGameObject()->GetComponentsInChildren<MapSector>())
    //{
    //    if (sector->type == MapSector::SectorType::Normal)
    //    {
    //        sectors.push_back(sector);
    //    }
    //}
    size_t size = sectors.size();

    vector<MapHexTile*> tiles;
    tiles.reserve(size);

    for (size_t i = 0ull; i < size; ++i)
    {
        std::vector<GameObject*> foundOnes;
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
            vec3 avgPosition;
            size_t foundOnesSize = foundOnes.size();
            for (size_t i = 0ull; i < foundOnesSize; ++i)
            {
                avgPosition += foundOnes[i]->GetTransform()->GetGlobalPosition();
            }
            avgPosition /= foundOnes.size();

            GameObject* found = foundOnes[0];
            float distance = glm::distance(found->GetTransform()->GetGlobalPosition(), avgPosition);
            for (size_t i = 1ull; i < foundOnesSize; ++i)
            {
                float currentDistance = glm::distance(foundOnes[i]->GetTransform()->GetGlobalPosition(), avgPosition);
                if (currentDistance < distance)
                {
                    found = foundOnes[i];
                    distance = currentDistance;
                }
            }

            tiles.push_back(found->GetComponent<MapHexTile>());
        }
    }

    size = tiles.size();
    for (size_t outIndex = 0ull; outIndex < size; ++outIndex)
    {
        vec3 outPosition = tiles[outIndex]->GetTransform()->GetGlobalPosition();
        outPosition.y = 0.0f;

        for (size_t innerIndex = outIndex + 1ull; innerIndex < size; ++innerIndex)
        {
            if (outIndex == innerIndex)
                continue;

            vec3 innerPosition = tiles[innerIndex]->GetTransform()->GetGlobalPosition();
            innerPosition.y = 0.0f;

            float distance = glm::distance(outPosition, innerPosition);

            //connections[tiles[outIndex]].first.push_back(std::make_pair(tiles[innerIndex], distance));
            connections[tiles[outIndex]].first[tiles[innerIndex]] = distance;
            connections[tiles[outIndex]].second += distance;

            connections[tiles[innerIndex]].first[tiles[outIndex]] = distance;
            connections[tiles[innerIndex]].second += distance;
        }
    }

    size_t toRemoveNumber = sectors.size() - static_cast<int>(densityFactorPerSector * sectors.size() + 0.5f);

    while (toRemoveNumber > 0ull)
    {
        float shortest = std::numeric_limits<float>::max();
        MapHexTile* shortestTile = nullptr;

        for (auto& pair : connections)
        {
            if (pair.second.second < shortest)
            {
                shortest = pair.second.second;
                shortestTile = pair.first;
            }
        }

        for (auto& pair : connections[shortestTile].first)
        {
            connections[pair.first].first.erase(shortestTile);
            connections[pair.first].second -= pair.second;
        }

        connections.erase(shortestTile);
        --toRemoveNumber;
    }

    for (auto& pair : connections)
    {
        pair.first->type = MapHexTile::HexTileType::RadioStation;
        GameObject* instantiated = nullptr;
        if (PrefabManager::GetPrefab(prefabRadioStation->GetId()) != nullptr) {
            instantiated = SceneManager::CreateGameObject(prefabRadioStation, pair.first->GetTransform());
        }
        else {

            if (prefabPath != "") {
                prefabRadioStation = PrefabManager::LoadPrefab(prefabPath);
                instantiated = SceneManager::CreateGameObject(prefabRadioStation, pair.first->GetTransform());
            }
            else {
                instantiated = SceneManager::CreateGameObject(pair.first->GetTransform());
            }
        }

        //GameObject* instantiated = SceneManager::CreateGameObject(prefabRadioStation, tile->GetTransform());
        instantiated->SetIsStatic(true);
        //GameObject* instantiated = GameObject::Instantiate(prefabRadioStation, tile->GetTransform());
    }
}

void RadioStationGeneratorSectorBased::Clear() {
    prefabRadioStation = nullptr;
}