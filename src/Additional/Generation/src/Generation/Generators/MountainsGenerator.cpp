#include <Generation/Generators/MountainsGenerator.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;

using namespace std;
using namespace glm;

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

    for (int i = 0; i < mountainsNumber; i++)
    {
        size_t index = Random::Range(0ull, sectors.size() - 1);
        MapSector* sector = sectors[index];

        sector->type = MapSector::SectorType::Mountain;
        sector->GetTransform()->Translate(glm::vec3(0.0f, mountainsHeight, 0.0f));

        for (MapHexTile* tile : sector->GetTiles())
        {
            SPDLOG_WARN("Dodaæ warstwy w GameObjectach");
            //tile->layer = LayerMask::NameToLayer("Mountain");
            tile->type = MapHexTile::HexTileType::Mountain;

            GameObject* mountain = GameObject::Instatiate(prefabMountains, tile->GetGameObject()->GetTransform());
        }

        sectors.erase(sectors.begin() + index);
    }
}