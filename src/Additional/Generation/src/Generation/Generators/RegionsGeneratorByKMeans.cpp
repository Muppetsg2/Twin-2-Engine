#include <Generation/Generators/RegionsGeneratorByKMeans.h>

using namespace Algorithms;
using namespace Generation::Generators;
using namespace Tilemap;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace glm;
using namespace std;

SCRIPTABLE_OBJECT_SOURCE_CODE(RegionsGeneratorByKMeans, Generation::Generators, "RegionsGeneratorByKMeans")

SO_SERIALIZATION_BEGIN(RegionsGeneratorByKMeans, AMapElementGenerator)
SO_SERIALIZE_FIELD_F(regionPrefab, PrefabManager::GetPrefabPath)
SO_SERIALIZE_FIELD(regionsCount)
SO_SERIALIZE_FIELD(isDiscritizedHeight)
SO_SERIALIZE_FIELD(lowerHeightRange)
SO_SERIALIZE_FIELD(upperHeightRange)
SO_SERIALIZE_FIELD(heightRangeFacor)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(RegionsGeneratorByKMeans, AMapElementGenerator)
SO_DESERIALIZE_FIELD_F_T(regionPrefab, PrefabManager::GetPrefab, string)
SO_DESERIALIZE_FIELD(regionsCount)
SO_DESERIALIZE_FIELD(isDiscritizedHeight)
SO_DESERIALIZE_FIELD(lowerHeightRange)
SO_DESERIALIZE_FIELD(upperHeightRange)
SO_DESERIALIZE_FIELD(heightRangeFacor)
SO_DESERIALIZATION_END()




//template class ObjectsKMeans<GameObject*>;

void RegionsGeneratorByKMeans::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    ivec2 leftBottomPosition = tilemap->GetLeftBottomPosition();
    ivec2 rightTopPosition = tilemap->GetRightTopPosition();

    vector<GameObject*> gameObjectsInTilemap;

    for (int x = leftBottomPosition.x; x <= rightTopPosition.x; x++)
    {
        for (int y = leftBottomPosition.y; y <= rightTopPosition.y; y++)
        {
            ivec2 tilePosition(x, y);
            HexagonalTile* tile = tilemap->GetTile(tilePosition);
            GameObject* tileObject = tile->GetGameObject();
            if (tileObject != nullptr)
            {
                gameObjectsInTilemap.push_back(tileObject);
                //MapHexTile* hexTile = tileObject->GetComponent<MapHexTile>();
                //hexTile->tilemap = tilemap;
                //hexTile->tile = tile;
            }
        }
    }

    vector<vector<GameObject*>> regionGameObjectClusters = ObjectsKMeans::ClusterObjects<GameObject*>(regionsCount, gameObjectsInTilemap, [](GameObject* gameObject) { return gameObject->GetTransform()->GetGlobalPosition(); });

    for (const auto& cluster : regionGameObjectClusters)
    {
        if (!cluster.empty())
        {
            MapRegion* region = SceneManager::CreateGameObject(regionPrefab, tilemap->GetTransform())->GetComponent<MapRegion>();
            //MapRegion* region = GameObject::Instantiate(regionPrefab, tilemap->GetTransform())->GetComponent<MapRegion>();
            region->tilemap = tilemap;

            for (GameObject* gameObject : cluster)
            {
                //region->AddTile(gameObject);
                gameObject->GetTransform()->SetParent(region->GetTransform());
            }

            if (isDiscritizedHeight)
            {
                region->GetTransform()->Translate(vec3(0.0f, (int)Random::Range(lowerHeightRange, upperHeightRange) * heightRangeFacor, 0.0f));
            }
            else
            {
                region->GetTransform()->Translate(vec3(0.0f, Random::Range(lowerHeightRange, upperHeightRange), 0.0f));
            }
        }
    }
}
