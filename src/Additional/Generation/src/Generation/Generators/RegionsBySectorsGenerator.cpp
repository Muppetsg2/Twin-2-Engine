#include <Generation/Generators/RegionsBySectorsGenerator.h>

using namespace Generation;
using namespace Generation::Generators;

using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;
using namespace glm;

SCRIPTABLE_OBJECT_SOURCE_CODE(RegionsBySectorsGenerator, Generation::Generators, "RegionsBySectorsGenerator")

SO_SERIALIZATION_BEGIN(RegionsBySectorsGenerator, AMapElementGenerator)
SO_SERIALIZE_FIELD_F(regionPrefab, PrefabManager::GetPrefabPath)
SO_SERIALIZE_FIELD(mergeByNumberTilesPerRegion)
SO_SERIALIZE_FIELD(minTilesPerRegion)
SO_SERIALIZE_FIELD(maxTilesPerRegion)
SO_SERIALIZE_FIELD(minSectorsPerRegion)
SO_SERIALIZE_FIELD(maxSectorsPerRegion)
SO_SERIALIZE_FIELD(isDiscritizedHeight)
SO_SERIALIZE_FIELD(lowerHeightRange)
SO_SERIALIZE_FIELD(upperHeightRange)
SO_SERIALIZE_FIELD(heightRangeFacor)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(RegionsBySectorsGenerator, AMapElementGenerator)
SO_DESERIALIZE_FIELD_F_T(regionPrefab, PrefabManager::GetPrefab, string)
SO_DESERIALIZE_FIELD(mergeByNumberTilesPerRegion)
SO_DESERIALIZE_FIELD(minTilesPerRegion)
SO_DESERIALIZE_FIELD(maxTilesPerRegion)
SO_DESERIALIZE_FIELD(minSectorsPerRegion)
SO_DESERIALIZE_FIELD(maxSectorsPerRegion)
SO_DESERIALIZE_FIELD(isDiscritizedHeight)
SO_DESERIALIZE_FIELD(lowerHeightRange)
SO_DESERIALIZE_FIELD(upperHeightRange)
SO_DESERIALIZE_FIELD(heightRangeFacor)
SO_DESERIALIZATION_END()




void RegionsBySectorsGenerator::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    list<MapSector*> sectorsList = tilemap->GetGameObject()->GetComponentsInChildren<MapSector>();
    vector<MapSector*> sectors;
    sectors.reserve(sectorsList.size());
    for (MapSector* sector : sectorsList)
    {
        sectors.push_back(sector);
    }
    sectorsList.clear();

    list<MapRegion*> regions;

    for (MapSector* sector : sectors)
    {
        MapRegion* region = SceneManager::CreateGameObject(regionPrefab, tilemap->GetTransform())->GetComponent<MapRegion>();
        //MapRegion* region = GameObject::Instantiate(regionPrefab, tilemap->GetTransform())->GetComponent<MapRegion>();
        region->tilemap = tilemap;
        regions.push_back(region);
        region->AddSector(sector);
    }

    list<MapRegion*> allRegions(regions);

    if (mergeByNumberTilesPerRegion)
    {
        regions.remove_if([this](MapRegion* region) {
            return region->GetTilesCount() >= minTilesPerRegion;
            });
    }
    else
    {
        regions.remove_if([this](MapRegion* region) {
            return region->GetSectorsCount() >= minSectorsPerRegion;
            });
    }

    while (!regions.empty())
    {
        // Jest jakiœ b³¹d który powoduje ¿e w hierarchi pozostaj¹ puste sektory, prawdopodobnie brak usuwania sektorów oraz regionó z hierarc
        // Poni¿sze jest prawdopodobnie do usuniêcia, ale trzeba jeszcze to zdecydowaæ
        regions.remove_if([this](MapRegion* region) {
            return region->GetSectorsCount() == 0 || region->GetTilesCount() == 0;
            });

        MapRegion* chosenRegion = nullptr;

        list<MapRegion*> foundOnes;

        if (mergeByNumberTilesPerRegion)
        {
            auto smallestCount = min_element(regions.begin(), regions.end(), [](MapRegion* a, MapRegion* b) {
                return a->GetTilesCount() < b->GetTilesCount();
                });
            
            list<MapRegion*> smallest;// = regions.FindAll(region = > region.TilesCount == smallestCount);
            for (MapRegion* region : regions)
            {
                if (region->GetTilesCount() == (*smallestCount)->GetTilesCount())
                {
                    smallest.push_back(region);
                }
            }
            auto chosenItr = smallest.begin();
            std::advance(chosenItr, Random::Range(0ull, smallest.size() - 1));
            chosenRegion = *chosenItr;

            vector<MapRegion*> neighbouringRegions = chosenRegion->GetAdjacentRegions();

            auto minCount = min_element(neighbouringRegions.begin(), neighbouringRegions.end(), [](MapRegion* a, MapRegion* b) {
                return a->GetTilesCount() < b->GetTilesCount();
                });

            for (MapRegion* region : neighbouringRegions)
            {
                if (region->GetTilesCount() == (*minCount)->GetTilesCount())
                {
                    foundOnes.push_back(region);
                }
            }
            //chosenRegion = foundOnes[Random::Range(0ull, foundOnes.size() - 1)];
        }
        else
        {
            unsigned int smallestCount = (*min_element(regions.begin(), regions.end(), [](MapRegion* a, MapRegion* b) {
                return a->GetSectorsCount() < b->GetSectorsCount();
                }))->GetSectorsCount();

            list<MapRegion*> smallest;// = regions.FindAll(region = > region.TilesCount == smallestCount);
            for (MapRegion* region : regions)
            {
                if (region->GetSectorsCount() == smallestCount)
                {
                    smallest.push_back(region);
                }
            }
            auto chosenItr = smallest.begin();
            std::advance(chosenItr, Random::Range(0ull, smallest.size() - 1));
            chosenRegion = *chosenItr;

            vector<MapRegion*> neighbouringRegions(chosenRegion->GetAdjacentRegions());

            unsigned int minCount = (*min_element(neighbouringRegions.begin(), neighbouringRegions.end(), [](MapRegion* a, MapRegion* b) {
                return a->GetSectorsCount() < b->GetSectorsCount();
                }))->GetSectorsCount();

            for (MapRegion* region : neighbouringRegions)
            {
                if (region->GetSectorsCount() == minCount)
                {
                    foundOnes.push_back(region);
                }
            }
            //chosenRegion = foundOnes[Random::Range(0ull, foundOnes.size() - 1)];
        }
        auto foundOneItr = foundOnes.begin();
        advance(foundOneItr, Random::Range(0ull, foundOnes.size() - 1));
        MapRegion* chosen = *foundOneItr;

        chosen->JoinRegion(chosenRegion);
        regions.remove(chosenRegion);
        allRegions.remove(chosenRegion);
        SPDLOG_ERROR("Dodaæ usuwanie GameObjectu");
        chosenRegion->GetTransform()->SetParent(nullptr);
        SceneManager::DestroyGameObject(chosenRegion->GetGameObject());
        //DestroyImmediate(chosenRegion->GetGameObject());

        if (mergeByNumberTilesPerRegion)
        {
            regions.remove_if([this](MapRegion* region) {
                return region->GetTilesCount() >= minTilesPerRegion;
                });
        }
        else
        {
            regions.remove_if([this](MapRegion* region) {
                return region->GetSectorsCount() >= minSectorsPerRegion;
                });
        }
    }

    for (auto region : allRegions)
    {
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