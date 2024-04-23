#include <Generation/Generators/RegionsGeneratorByKMeans.h>

using namespace Algorithms;
using namespace Generation::Generators;
using namespace Tilemap;
using namespace Twin2Engine::Core;

using namespace glm;
using namespace std;


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
            MapRegion* region = GameObject::Instantiate(regionPrefab, tilemap->GetTransform())->GetComponent<MapRegion>();
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
