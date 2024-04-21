#include <Generation/Generators/SectorGeneratorForRegionsByKMeans.h>

using namespace std;
using namespace Algorithms;
using namespace Generation::Generators;
using namespace Tilemap;
using namespace Twin2Engine::Core;
using namespace glm;

//template class ObjectsKMeans<MapHexTile*>;

void SectorGeneratorForRegionsByKMeans::Generate(HexagonalTilemap* tilemap) 
{
    list<MapRegion*> regions(tilemap->GetGameObject()->GetComponentsInChildren<MapRegion>());

    for (MapRegion* region : regions)
    {
        list<MapHexTile*> tilesList(region->GetGameObject()->GetComponentsInChildren<MapHexTile>());
        vector<MapHexTile*> tiles(tilesList.begin(), tilesList.end());
        tilesList.clear();

        //ObjectsKMeans<MapHexTile*> kMeans;
        vector<vector<MapHexTile*>> regionTilesClusters = ObjectsKMeans::ClusterObjects<MapHexTile*>(sectorsCount, tiles, [](MapHexTile* hexTile) { return hexTile->GetTransform()->GetGlobalPosition(); });

        for (const auto& cluster : regionTilesClusters)
        {
            if (!cluster.empty()) 
            {
                MapSector* sector = GameObject::Instantiate(sectorPrefab, tilemap->GetTransform())->GetComponent<MapSector>();
                sector->tilemap = tilemap;
                sector->region = region;

                for (MapHexTile* tile : cluster)
                {
                    sector->AddTile(tile);
                }

                region->AddSector(sector);

                if (isDiscritizedHeight)
                {
                    sector->GetTransform()->Translate(vec3(0.0f, (int)Random::Range(lowerHeightRange, upperHeightRange) * heightRangeFacor, 0.0f));
                }
                else
                {
                    sector->GetTransform()->Translate(vec3(0.0f, Random::Range(lowerHeightRange, upperHeightRange), 0.0f));
                }
            }
        }
    }
}
