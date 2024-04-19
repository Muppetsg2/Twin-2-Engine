#include <Generation/Generators/SectorsGenerator.h>

using namespace Generation;
using namespace Generation::Generators;
using namespace Tilemap;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace glm;

using namespace std;


void SectorsGenerator::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    list<MapSector*> sectors;

    ivec2 lbPosition = tilemap->GetLeftBottomPosition();
    ivec2 rtPosition = tilemap->GetRightTopPosition();
    //*
    vector<ivec2> positions;//mo¿na wy u¿yæ jakiegoœ resezie lub reserbe
    positions.reserve(tilemap->GetWidth() * tilemap->GetHeight());
    // robiæ w tilemapie zliczanie poll aktwnych w tilemapie.
    
    for (int x = lbPosition.x; x <= rtPosition.x; x++)
    {
        for (int y = lbPosition.y; y <= rtPosition.y; y++)
        {
            ivec2 tilePosition(x, y);
            positions.push_back(tilePosition);
        }
    }

    for (int i = 0; i < positions.size(); i++)
    {
        HexagonalTile* tile = tilemap->GetTile(positions[i]);
        GameObject* tileObj = tile->GetGameObject();
        if (tileObj != nullptr)
        {
            MapHexTile* hexTile = tileObj->GetComponent<MapHexTile>();
            if (hexTile->sector == nullptr)
            {
                sectors.push_back(CreateSector(tilemap, positions[i]));
            }
        }
    }

    list<MapSector*> smallSectors;
    
    int minTilesNumber = minTilesPerSector;
    float accuracy = accuracyFactor;

    std::copy_if(sectors.begin(), sectors.end(), back_inserter(smallSectors), [minTilesNumber, accuracy](const MapSector* sector) { return sector->GetTilesCount() < (minTilesNumber * accuracy); });

    while (smallSectors.size() > 0)
    {
        smallSectors.sort([](const MapSector* sector1, const MapSector* sector2) { return sector1->GetTilesCount() < sector2->GetTilesCount(); });
        //sort(smallSectors.cbegin(), smallSectors.cend(), );
        vector<MapSector*> adjacentSectors(smallSectors.front()->GetAdjacentSectors());
    
        //int minCount = (*min_element(adjacentSectors.begin(), adjacentSectors.end(), [](const MapSector* sector) { return sector->GetTilesCount(); }))->GetTilesCount();
        int minCount = (*min_element(adjacentSectors.begin(), adjacentSectors.end(), [](MapSector* sector1, MapSector* sector2) { return sector1->GetTilesCount() < sector2->GetTilesCount(); }))->GetTilesCount();
    
        list<MapSector*> foundOnes;
        std::copy_if(adjacentSectors.begin(), adjacentSectors.end(), back_inserter(foundOnes), [minCount](const MapSector* sector) { return sector->GetTilesCount() == minCount; });
    
        auto chosenItr = foundOnes.begin();
        advance(chosenItr, Random::Range(0ull, foundOnes.size() - 1));
        MapSector* chosen = *chosenItr;
    
        chosen->JoinSector(smallSectors.front());
        MapSector* toDestroy = smallSectors.front();
        //smallRegions.RemoveAt(0);
        sectors.remove(toDestroy);
    
        SPDLOG_ERROR("Stworzyæ Destroy oraz DestroyImmediate dla GameObject");
        toDestroy->GetTransform()->SetParent(nullptr);
        SceneManager::DestroyGameObject(toDestroy->GetGameObject());
        //DestroyImmediate(toDestroy.gameObject);
    
        //smallSectors = sectors.FindAll(sector => sector.sectorTiles.Count < (minTilesPerSector * accuracyFactor) && sector.sectorTiles.Count != 0);
        smallSectors.clear();
        std::copy_if(sectors.begin(), sectors.end(), back_inserter(smallSectors), [minTilesNumber, accuracy](const MapSector* sector) { return sector->GetTilesCount() < (minTilesNumber * accuracy); });
    
    }/**/
}

struct Counter
{
    MapHexTile* tile;
    int count = 0;
};

MapSector* SectorsGenerator::CreateSector(Tilemap::HexagonalTilemap* tilemap, glm::ivec2 position)
{
    list<Counter> possibleTiles;

    GameObject* possibilities[6];
    tilemap->GetTile(position)->GetAdjacentGameObjects(possibilities);
    for (int j = 0; j < 6; j++)
    {
        //if (tile != null && tile.transform.parent == tilemap.transform)
        if (possibilities[j] != nullptr)
        {
            MapHexTile* hexTile = possibilities[j]->GetComponent<MapHexTile>();
            if (hexTile->sector == nullptr)
            {
                possibleTiles.push_back(Counter{ .tile = hexTile, .count = 1 });
            }
        }
    }

    int tilesInSector = Random::Range(minTilesPerSector, maxTilesPerSector);
    MapHexTile* chosenTile = nullptr;
    list<MapHexTile*> sectorTiles;
    sectorTiles.push_back(tilemap->GetTile(position)->GetGameObject()->GetComponent<MapHexTile>());

    for (int i = 0; i < tilesInSector && possibleTiles.size() != 0; i++)
        //for (int i = 0; regionPositions.Count < tilesInSector && possibleTiles.Count != 0; i++)
    {
        //int maxCount = int.MinValue; // Initialize to smallest possible integer value
        //foreach (Counter counter in possibleTiles)
        //{
        //    if (counter.count > maxCount)
        //    {
        //        maxCount = counter.count;
        //    }
        //}

        //int maxCount = possibleTiles.Max(counter = > counter.count);
        int maxCount = max_element(possibleTiles.begin(), possibleTiles.end(), [](const Counter& counter1, const Counter& counter2) { return counter1.count > counter2.count; })->count;


        list<Counter> foundOnes;

        std::copy_if(possibleTiles.begin(), possibleTiles.end(), back_inserter(foundOnes), [maxCount](const Counter& counter) { return counter.count == maxCount; });

        auto itr = foundOnes.begin();
        std::advance(itr, Random::Range(0, (int)foundOnes.size() - 1));
        Counter chosen = (*itr);
        chosenTile = chosen.tile;
        sectorTiles.push_back(chosenTile);
        possibleTiles.remove_if([itr](const Counter& counter) { return counter.tile == itr->tile; });

        chosen.tile->tile->GetAdjacentGameObjects(possibilities);
        for (int j = 0; j < 6; j++)
        {
            //if (tile != null && tile.transform.parent == tilemap.transform)
            if (possibilities[j] != nullptr)
            {
                MapHexTile* hexTile = possibilities[j]->GetComponent<MapHexTile>();
                if (hexTile->sector == nullptr)
                {
                    //auto found = possibleTiles.begin();//find(possibleTiles.begin(), possibleTiles.end(), [hexTile](Counter tile) { return tile.tile == hexTile; });
                    //auto found = find(possibleTiles.begin(), possibleTiles.end(), [hexTile](Counter tile) { return tile.tile == hexTile; });
                    MapHexTile* foundTile = nullptr;
                    for (Counter& counter : possibleTiles)
                    {
                        if (counter.tile == hexTile)
                        {
                            foundTile = counter.tile;
                            counter.count++;
                            break;
                        }
                    }
                    
                    //if (found == possibleTiles.end())
                    if (foundTile == nullptr)
                    {
                        possibleTiles.push_back(Counter{ .tile = hexTile, .count = 1 });
                    }
                    //else
                    //{
                    //    found->count++;
                    //}

                }
            }
        }
    }

    GameObject* sectorGO = GameObject::Instantiate(prefabSector, tilemap->GetTransform());
    MapSector* sector = sectorGO->GetComponent<MapSector>();

    sector->tilemap = tilemap;
    sector->GetTransform()->SetParent(tilemap->GetTransform());

    for (auto tile : sectorTiles)
    {
        sector->AddTile(tile);
    }

    return sector;
}