#include <Generation/MapGenerator.h>

using namespace Generation;
using namespace Twin2Engine::Core;
using namespace std;
using namespace glm;

void MapGenerator::GenerateFloatHull(const vector<vec2>& hull)
{
    vector<ivec2> hullInt(hull.size());

    for (int i = 0; i < hull.size(); i++)
    {
        hullInt[i] = ivec2(hull[i].x + 0.5f, hull[i].y + 0.5f);
    }

    for (int i = 0; i < (hull.size() - 1); i++)
    {
        ConnectTiles(hullInt[i], hullInt[i + 1]);
    }
    ConnectTiles(hullInt[hullInt.size() - 1], hullInt[0]);
}


void MapGenerator::GeneratePositions(const vector<ivec2>& positions)
{
    for (int i = 0; i < positions.size(); i++)
    {
        //tilemap->SetTile(positions[i], preafabHexagonalTile);
        tilemap->SetTile(positions[i], pointTile);
    }
}

void MapGenerator::GenerateForFloatPositions(const vector<vec2>& positions)
{
    vector<ivec2> temp(positions.size());

    for (int i = 0; i < positions.size(); i++)
    {
        temp[i] = ivec2(positions[i].x + 0.5f, positions[i].y + 0.5f);
    }


    GeneratePositions(temp);
}

void MapGenerator::ConnectTiles(ivec2 startTile, ivec2 endTile)
{
    int dx = endTile.x - startTile.x;
    int dy = endTile.y - startTile.y;
    //int dz = endTile.z - startTile.z;

    int steps = max(abs(dx), abs(dy)); // Zastanowiæ siê czy to jakoœ nie zmieniæ z makr na funckje
    //int steps = Mathf.Max(Mathf.Abs(dx), Mathf.Abs(dy), Mathf.Abs(dz));

    float delta_x = dx / (float)steps;
    float delta_y = dy / (float)steps;
    //float delta_z = dz / (float)steps;

    float x = startTile.x;
    float y = startTile.y;
    //float z = startTile.z;


    //float q = startTile.x;
    //float r = startTile.y;

    //ivec2 lastCellPosition = new(Mathf.RoundToInt(x), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
    ivec2 lastCellPosition(x + 0.5f, y + 0.5f);
    ivec2 cellPosition;
    for (int i = 0; i <= steps; i++)
    {
        //ivec3 cellPosition = new ivec3(Mathf.RoundToInt(x) + (Mathf.RoundToInt(y) % 2), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
        //ivec3 cellPosition = new(Mathf.RoundToInt(x), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
        cellPosition = ivec2(x + 0.5f, y + 0.5f);
        if (cellPosition.y != lastCellPosition.y && cellPosition.x != lastCellPosition.x)
        {
            if (abs(cellPosition.y % 2) == 0 && cellPosition.x < lastCellPosition.x)
            {
                //tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), preafabHexagonalTile);
                tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), additionalTile);

                //tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), preafabHexagonalTile);
                tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), additionalTile);

            }
            else if (abs(cellPosition.y % 2) == 1 && cellPosition.x > lastCellPosition.x)
            {
                //tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), preafabHexagonalTile);
                tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), additionalTile);

                //tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), preafabHexagonalTile);
                tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), additionalTile);
            }
        }
        lastCellPosition = cellPosition;
        tilemap->SetTile(cellPosition, preafabHexagonalTile);


        SPDLOG_INFO("Generated position: {} {}", cellPosition.x, cellPosition.y);

        x += delta_x;
        y += delta_y;
        //z += delta_z;
    }
}

void MapGenerator::GenerateRandomHull()
{
    int pointsNumber = Random::Range(minPointsNumber, maxPointsNumber);
    float angleDelta = 2 * pi<float>() / pointsNumber;
    float radiansAngleDeltaRange = radians(angleDeltaRange);
    float angle = 0.0f;

    vector<vec2> hexPositions(pointsNumber);

    for (int i = 0; i < pointsNumber; i++)
    {
        float usedAngle = angle + Twin2Engine::Core::Random::Range(-radiansAngleDeltaRange, radiansAngleDeltaRange);

        hexPositions[i] = vec2(cos(usedAngle), sin(usedAngle)) * (generationRadius + Twin2Engine::Core::Random::Range(generationRadiusMinModifier, generationRadiusMaxModifier));

        angle += angleDelta;
    }

    GenerateFloatHull(hexPositions);

    //if (debugMode)
    //{
    //    GenerateForFloatPositions(hexPositions);
    //}
    GenerateForFloatPositions(hexPositions);
}

void MapGenerator::Generate()
{
    //if (!debugMode)
    //{
    //    additionalTile = tile;
    //    filledTile = tile;
    //    pointTile = tile;
    //}

    GenerateRandomHull();

    //if (!debugMode)
    //{
    //
    //    tilemap->Fill(ivec3.zero, filledTile);
    //}
   tilemap->Fill(ivec2(0, 0), filledTile);

    //tilemapRegionDivider = GetComponent<ITilemapRegionDivider>();
    //tilemapRegionDivider.DivideTilemap();


    //BoundsInt bounds = tilemap.cellBounds;
    ivec2 leftBottomPosition = tilemap->GetLeftBottomPosition();
    ivec2 rightTopPosition = tilemap->GetRightTopPosition();

    for (int x = leftBottomPosition.x; x < rightTopPosition.x; x++)
    {
        for (int y = leftBottomPosition.y; y < rightTopPosition.y; y++)
        {
            ivec2 tilePosition(x, y);
            Twin2Engine::Core::GameObject* tileObject = tilemap->GetTile(tilePosition)->GetGameObject();
            if (tileObject != nullptr)
            {
                //HexTile hexTile = tileObject.GetComponent<HexTile>();
                //hexTile->tilemap = tilemap;
                //hexTile->tilemapPosition = tilePosition;
            }
        }
    }
    
    SPDLOG_ERROR("Zrobiæ implementacjê content generator!");
    //GetComponent<IContentGenerator>() ? .GenerateContent();
}