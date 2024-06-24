#include <Generation/Generators/MapValidator.h>

#include <manager/SceneManager.h>

#include <AstarPathfinding/AStarPathfinder.h>

#include <Generation/ContentGenerator.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace AStar;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(MapValidator, Generation::Generators, "MapValidator")

SO_SERIALIZATION_BEGIN(MapValidator, AMapElementGenerator)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(MapValidator, AMapElementGenerator)
SO_DESERIALIZATION_END()


void MapValidator::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    list<MapSector*> sectors = tilemap->GetGameObject()->GetComponentsInChildren<MapSector>();

    size_t sectorsNumber = sectors.size();
    vector<MapHexTile*> tiles;
    tiles.reserve(sectorsNumber);

    list<MapSector*>::const_iterator citr = sectors.cbegin();
    for (size_t index = 0ull; index < sectorsNumber; ++index, ++citr)
    {
        tiles.push_back(*(*citr)->GetTiles().begin());
    }

    vector<AStarPathfindingInfo> pathfindingInfos;
    pathfindingInfos.resize(sectorsNumber - 1ull);

    bool incorrect = false;
    for (size_t index2 = 1ull; index2 < sectorsNumber; ++index2)
    {
        pathfindingInfos[index2 - 1ull] = AStarPathfinder::FindNodePath(
            tiles[index2]->GetTransform()->GetGlobalPosition(),
            tiles[0ull]->GetTransform()->GetGlobalPosition(),
            0,
            [&](const AStarNodePath& path) -> void {},
            [&]() -> void { incorrect = true; }
        );
    }

    for (size_t index3 = 0ull; index3 < (sectorsNumber - 1ull); ++index3)
    {
        pathfindingInfos[index3].WaitForFinding();
    }

    if (incorrect)
    {
        tilemap->GetGameObject()->GetComponent<ContentGenerator>()->Reset();
    }
}

void MapValidator::Clear() {

}