#include <Generation/Generators/CitiesGenerator.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(CitiesGenerator, Generation::Generators, "CitiesGenerator")

SO_SERIALIZATION_BEGIN(CitiesGenerator, AMapElementGenerator)
    SO_SERIALIZE_FIELD_F(prefabCity, PrefabManager::GetPrefabPath)
    SO_SERIALIZE_FIELD(byRegions)
    SO_SERIALIZE_FIELD(density)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(CitiesGenerator, AMapElementGenerator)
    SO_DESERIALIZE_FIELD_R("prefabCity", prefabPath)
    SO_DESERIALIZE_FIELD_F_T(prefabCity, PrefabManager::LoadPrefab, string)
    SO_DESERIALIZE_FIELD(byRegions)
    SO_DESERIALIZE_FIELD(density)
SO_DESERIALIZATION_END()

//void CitiesGenerator::Serialize(YAML::Node& node) const
//{
//    AMapElementGenerator::Serialize(node);
//
//    node["byRegions"] = byRegions;
//    node["density"] = density;
//}
//bool CitiesGenerator::Deserialize(const YAML::Node& node)
//{
//    bool baseReturned = AMapElementGenerator::Deserialize(node);
//
//    byRegions = node["byRegions"].as<decltype(byRegions)>();
//    density = node["density"].as<decltype(density)>();
//
//    return baseReturned;
//}

void CitiesGenerator::Generate(HexagonalTilemap* tilemap)
{
    if (!prefabCity)
    {
        prefabCity = PrefabManager::LoadPrefab(prefabPath);
    }

    /*
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

        size_t size = static_cast<int>(density * regions.size());
        for (size_t i = 0ull; i < size; ++i)
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
                found->GetComponent<MapHexTile>()->type = MapHexTile::HexTileType::PointOfInterest;
                //GameObject* instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());
                GameObject* instantiated = nullptr;
                if (PrefabManager::GetPrefab(prefabCity->GetId()) != nullptr) {
                    instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());
                }
                else {

                    if (prefabPath != "") {
                        prefabCity = PrefabManager::LoadPrefab(prefabPath);
                        instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());
                    }
                    else {
                        instantiated = std::get<0>(SceneManager::CreateGameObject<City>(found->GetTransform()));
                    }
                }
                instantiated->SetIsStatic(true);
                //GameObject* instantiated = GameObject::Instantiate(prefabCity, found->GetTransform());
                CitiesManager::AddCity(instantiated);
                instantiated->GetComponent<City>()->StartAffectingTiles(found->GetComponent<HexTile>());
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

        size_t size = static_cast<int>(density * sectors.size());
        for (size_t i = 0ull; i < size; ++i)
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
                size_t foundOnesSize = foundOnes.size();
                for (size_t i = 0ull; i < foundOnesSize; ++i)
                {
                    avgPosition += foundOnes[i]->GetTransform()->GetGlobalPosition();
                }
                avgPosition /= foundOnesSize;

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
                found->GetComponent<MapHexTile>()->type = MapHexTile::HexTileType::PointOfInterest;

                GameObject* instantiated = nullptr;
                if (PrefabManager::GetPrefab(prefabCity->GetId()) != nullptr) {
                    instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());
                }
                else {

                    if (prefabPath != "") {
                        prefabCity = PrefabManager::LoadPrefab(prefabPath);
                        instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());
                    }
                    else {
                        instantiated = SceneManager::CreateGameObject(found->GetTransform());
                    }
                }

                //GameObject* instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());

                instantiated->SetIsStatic(true);
                CitiesManager::AddCity(instantiated);
                instantiated->GetComponent<City>()->StartAffectingTiles(found->GetComponent<HexTile>());
            }
        }
    }
    /**/

    if (byRegions)
    {
        unordered_map<MapHexTile*, pair<unordered_map<MapHexTile*, float>, float>> connections;

        vector<MapRegion*> regions;

        for (MapRegion* region : tilemap->GetGameObject()->GetComponentsInChildren<MapRegion>())
        {
            if (region->type == MapRegion::RegionType::Normal)
            {
                regions.push_back(region);
            }
        }
        size_t size = regions.size();

        vector<MapHexTile*> tiles;
        tiles.reserve(size);

        for (size_t i = 0ull; i < size; ++i)
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

        size_t toRemoveNumber = regions.size() - static_cast<int>(density * regions.size() + 0.5f);

        while (toRemoveNumber > 0ull)
        {
            float shortest = std::numeric_limits<float>::max();
            float longest = -1.0f;
            MapHexTile* shortestTile = nullptr;
            MapHexTile* longestTile = nullptr;

            float length = 0.0f;

            for (auto& pair : connections)
            {
                length += pair.second.second;

                if (pair.second.second < shortest)
                {
                    shortest = pair.second.second;
                    shortestTile = pair.first;
                }
                else if (pair.second.second > longest)
                {
                    longest = pair.second.second;
                    longestTile = pair.first;
                }
            }

            length /= connections.size() * 2;

            MapHexTile* processedTile = nullptr;
            if ((longest - length) > (length - shortest))
            {
                processedTile = longestTile;
            }
            else
            {
                processedTile = shortestTile;
            }

            for (auto& pair : connections[processedTile].first)
            {
                connections[pair.first].first.erase(processedTile);
                connections[pair.first].second -= pair.second;
            }

            connections.erase(processedTile);
            --toRemoveNumber;
        }

        for (auto& pair : connections)
        {
            pair.first->type = MapHexTile::HexTileType::PointOfInterest;
            //GameObject* instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());
            GameObject* instantiated = nullptr;
            if (PrefabManager::GetPrefab(prefabCity->GetId()) != nullptr) {
                instantiated = SceneManager::CreateGameObject(prefabCity, pair.first->GetTransform());
            }
            else {

                if (prefabPath != "") {
                    prefabCity = PrefabManager::LoadPrefab(prefabPath);
                    instantiated = SceneManager::CreateGameObject(prefabCity, pair.first->GetTransform());
                }
                else {
                    instantiated = std::get<0>(SceneManager::CreateGameObject<City>(pair.first->GetTransform()));
                }
            }
            instantiated->SetIsStatic(true);
            //GameObject* instantiated = GameObject::Instantiate(prefabCity, found->GetTransform());
            CitiesManager::AddCity(instantiated);
            instantiated->GetComponent<City>()->StartAffectingTiles(pair.first->GetGameObject()->GetComponent<HexTile>());
        }
    }
    else
    {
        unordered_map<MapHexTile*, pair<unordered_map<MapHexTile*, float>, float>> connections;

        vector<MapSector*> sectors;

        for (MapSector* sector : tilemap->GetGameObject()->GetComponentsInChildren<MapSector>())
        {
            if (sector->type == MapSector::SectorType::Normal)
            {
                sectors.push_back(sector);
            }
        }
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

        size_t toRemoveNumber = sectors.size() - static_cast<int>(density * sectors.size() + 0.5f);

        while (toRemoveNumber > 0ull)
        {
            float shortest = std::numeric_limits<float>::max();
            float longest = -1.0f;
            MapHexTile* shortestTile = nullptr;
            MapHexTile* longestTile = nullptr;

            float length = 0.0f;

            for (auto& pair : connections)
            {
                length += pair.second.second;

                if (pair.second.second < shortest)
                {
                    shortest = pair.second.second;
                    shortestTile = pair.first;
                }
                else if (pair.second.second > longest)
                {
                    longest = pair.second.second;
                    longestTile = pair.first;
                }
            }

            length /= connections.size() * 2;

            MapHexTile* processedTile = nullptr;
            if ((longest - length) > (length - shortest))
            {
                processedTile = longestTile;
            }
            else
            {
                processedTile = shortestTile;
            }

            for (auto& pair : connections[processedTile].first)
            {
                connections[pair.first].first.erase(processedTile);
                connections[pair.first].second -= pair.second;
            }

            connections.erase(processedTile);
            --toRemoveNumber;
        }

        for (auto& pair : connections)
        {
            pair.first->type = MapHexTile::HexTileType::PointOfInterest;
            //GameObject* instantiated = SceneManager::CreateGameObject(prefabCity, found->GetTransform());
            GameObject* instantiated = nullptr;
            if (PrefabManager::GetPrefab(prefabCity->GetId()) != nullptr) {
                instantiated = SceneManager::CreateGameObject(prefabCity, pair.first->GetTransform());
            }
            else {

                if (prefabPath != "") {
                    prefabCity = PrefabManager::LoadPrefab(prefabPath);
                    instantiated = SceneManager::CreateGameObject(prefabCity, pair.first->GetTransform());
                }
                else {
                    instantiated = std::get<0>(SceneManager::CreateGameObject<City>(pair.first->GetTransform()));
                }
            }
            instantiated->SetIsStatic(true);
            //GameObject* instantiated = GameObject::Instantiate(prefabCity, found->GetTransform());
            CitiesManager::AddCity(instantiated);
            instantiated->GetComponent<City>()->StartAffectingTiles(pair.first->GetGameObject()->GetComponent<HexTile>());
        }
    }

}

void CitiesGenerator::Clear()
{
    prefabCity = nullptr;
    CitiesManager::RemoveAllCities();
}