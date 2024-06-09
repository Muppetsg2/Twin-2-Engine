#include <Generation/Generators/SectorGeneratorForRegionsByKMeans.h>

using namespace std;
using namespace Algorithms;
using namespace Generation::Generators;
using namespace Tilemap;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(SectorGeneratorForRegionsByKMeans, Generation::Generators, "SectorGeneratorForRegionsByKMeans")

SO_SERIALIZATION_BEGIN(SectorGeneratorForRegionsByKMeans, AMapElementGenerator)
SO_SERIALIZE_FIELD_F(sectorPrefab, PrefabManager::GetPrefabPath)
SO_SERIALIZE_FIELD(sectorsCount)
SO_SERIALIZE_FIELD(isDiscritizedHeight)
SO_SERIALIZE_FIELD(lowerHeightRange)
SO_SERIALIZE_FIELD(upperHeightRange)
SO_SERIALIZE_FIELD(heightRangeFacor)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(SectorGeneratorForRegionsByKMeans, AMapElementGenerator)
SO_DESERIALIZE_FIELD_F_T(sectorPrefab, PrefabManager::LoadPrefab, string)
SO_DESERIALIZE_FIELD(sectorsCount)
SO_DESERIALIZE_FIELD(isDiscritizedHeight)
SO_DESERIALIZE_FIELD(lowerHeightRange)
SO_DESERIALIZE_FIELD(upperHeightRange)
SO_DESERIALIZE_FIELD(heightRangeFacor)
SO_DESERIALIZATION_END()

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
                MapSector* sector = SceneManager::CreateGameObject(sectorPrefab, tilemap->GetTransform())->GetComponent<MapSector>();
                //MapSector* sector = GameObject::Instantiate(sectorPrefab, tilemap->GetTransform())->GetComponent<MapSector>();
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

void SectorGeneratorForRegionsByKMeans::Clear() {

}