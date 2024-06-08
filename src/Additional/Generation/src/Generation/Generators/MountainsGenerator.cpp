#include <Generation/Generators/MountainsGenerator.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;
using namespace AStar;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(MountainsGenerator, Generation::Generators, "MountainsGenerator")

SO_SERIALIZATION_BEGIN(MountainsGenerator, AMapElementGenerator)
SO_SERIALIZE_FIELD_F(prefabMountains, PrefabManager::GetPrefabPath)
SO_SERIALIZE_FIELD(mountainsNumber)
SO_SERIALIZE_FIELD(mountainsHeight)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(MountainsGenerator, AMapElementGenerator)
SO_DESERIALIZE_FIELD_F_T(prefabMountains, PrefabManager::LoadPrefab, string)
SO_DESERIALIZE_FIELD(mountainsNumber)
SO_DESERIALIZE_FIELD(mountainsHeight)
SO_DESERIALIZATION_END()



void MountainsGenerator::Generate(HexagonalTilemap* tilemap)
{
    vector<MapSector*> sectors;
    for (MapSector* sector : tilemap->GetGameObject()->GetComponentsInChildren<MapSector>())
    {
        if (sector->type == MapSector::SectorType::Normal)
        {
            sectors.push_back(sector);
        }
    }

    for (int i = 0; i < mountainsNumber; ++i)
    {
        size_t index = Random::Range(0ull, sectors.size() - 1);
        MapSector* sector = sectors[index];

        sector->type = MapSector::SectorType::Mountain;
        sector->GetTransform()->Translate(glm::vec3(0.0f, mountainsHeight, 0.0f));

        for (MapHexTile* tile : sector->GetTiles())
        {
            //SPDLOG_WARN("Dodaæ warstwy w GameObjectach");
            //tile->layer = LayerMask::NameToLayer("Mountain");
            tile->type = MapHexTile::HexTileType::Mountain;
            tile->GetGameObject()->GetComponent<AStarPathfindingNode>()->passable = false;

            GameObject* mountain = SceneManager::CreateGameObject(prefabMountains, tile->GetGameObject()->GetTransform());
            mountain->SetIsStatic(true);
            //GameObject* mountain = GameObject::Instantiate(prefabMountains, tile->GetGameObject()->GetTransform());
        }

        sectors.erase(sectors.begin() + index);
    }
}