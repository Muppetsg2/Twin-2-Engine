#include "SectorGeneratorForRegionsByKMeans.h"

using namespace std;
using namespace Algorithms;
using namespace Generation::Generators;
using namespace Tilemap;
using namespace Twin2Engine::Core;
using namespace glm;

void SectorGeneratorForRegionsByKMeans::Generate(Tilemap::HexagonalTilemap* tilemap) 
{
    vector<MapRegion*> regions = tilemap->GetTransform()->GetComponentsInChildren<MapRegion>();

    for (MapRegion* region : regions)
    {
        vector<MapHexTile*> tiles = region->GetTransform()->GetComponentsInChildren<HexTile>();
        vector<vector<MapHexTile*>> regionTilesClusters = ObjectsKMeans<MapHexTile*>::ClusterObjects(sectorsCount, tiles, [](MapHexTile* hexTile) { return hexTile->GetTransform()->GetGlobalPosition(); });

        for (const auto& cluster : regionTilesClusters)
        {
            if (!cluster.empty()) 
            {
                MapSector* sector = GameObject::Instantiate<MapSector>(sectorPrefab, tilemap->GetTransform());
                sector->SetTilemap(tilemap);
                sector->SetRegion(region);

                for (HexTile* tile : cluster)
                {
                    sector->AddTile(tile->GetGameObject());
                }

                region->AddSector(sector);

                if (isDiscritizedHeight)
                {
                    sector->GetTransform()->Translate(vec3(0.0f, 0.0f, -(int)Random::Range(lowerHeightRange, upperHeightRange) * heightRangeFacor));
                }
                else
                {
                    sector->GetTransform()->Translate(vec3(0.0f, 0.0f, -Random::Range(lowerHeightRange, upperHeightRange)));
                }
            }
        }
    }
}
