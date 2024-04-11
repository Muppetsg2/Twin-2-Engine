#ifndef _MAP_GENERATOR_H_
#define _MAP_GENERATOR_H_

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Random.h>

#include <Tilemap/HexagonalTile.h>
#include <Tilemap/HexagonalTilemap.h>

namespace Generation
{
	class MapGenerator : public ::Twin2Engine::Core::Component
	{
        #pragma region COMPONENT_PARAMETERS

        Tilemap::HexagonalTilemap* tilemap;
        //Tilemap::HexagonalTile* tile;
        Twin2Engine::Core::GameObject* preafabHexagonalTile;


        float generationRadius = 5.0f;
        float generationRadiusMinModifier = 0.0f;
        float generationRadiusMaxModifier = 0.0f;

        int minPointsNumber = 5;
        int maxPointsNumber = 5;
        float angleDeltaRange = 5.0f;


        #pragma endregion


        void GenerateFloatHull(const std::vector<glm::vec2>& hull)
        {
            std::vector<glm::ivec2> hullInt(hull.size());

            for (int i = 0; i < hull.size(); i++)
            {
                hullInt[i] = glm::ivec2(hull[i].x + 0.5f, hull[i].y + 0.5f);
            }

            for (int i = 0; i < (hull.size() - 1); i++)
            {
                ConnectTiles(hullInt[i], hullInt[i + 1]);
            }
            ConnectTiles(hullInt[hullInt.size() - 1], hullInt[0]);
        }


        void GeneratePositions(const std::vector<glm::ivec2>& positions)
        {
            for (int i = 0; i < positions.size(); i++)
            {
                tilemap->SetTile(positions[i], preafabHexagonalTile);
            }
        }
        
        void GenerateForFloatPositions(const std::vector<glm::vec2>& positions)
        {
            std::vector<glm::ivec2> temp(positions.size());

            for (int i = 0; i < positions.size(); i++)
            {
                temp[i] = glm::ivec2(positions[i].x + 0.5f, positions[i].y + 0.5f);
            }


            GeneratePositions(temp);
        }
    public:
        void Awake()
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
            //    tilemap->Fill(glm::ivec3.zero, filledTile);
            //}
            tilemap->Fill(glm::ivec2(0, 0), preafabHexagonalTile);

            //tilemapRegionDivider = GetComponent<ITilemapRegionDivider>();
            //tilemapRegionDivider.DivideTilemap();


            //BoundsInt bounds = tilemap.cellBounds;
            glm::ivec2 leftBottomPosition = tilemap->GetLeftBottomPosition();
            glm::ivec2 rightTopPosition = tilemap->GetRightTopPosition();

            for (int x = leftBottomPosition.x; x < rightTopPosition.x; x++)
            {
                for (int y = leftBottomPosition.y; y < rightTopPosition.y; y++)
                {
                    glm::ivec2 tilePosition(x, y);
                    Twin2Engine::Core::GameObject* tileObject = tilemap->GetTile(tilePosition)->GetGameObject();
                    if (tileObject != nullptr)
                    {
                        //HexTile hexTile = tileObject.GetComponent<HexTile>();
                        //hexTile->tilemap = tilemap;
                        //hexTile->tilemapPosition = tilePosition;
                    }
                }
            }

            spdlog::error("Zrobiæ implementacjê content generator!");
            //GetComponent<IContentGenerator>() ? .GenerateContent();
        }

        //void Fill()
        //{
        //    tilemap->Fill(glm::ivec2(0, 0), filledTile);
        //}


        void ConnectTiles(glm::ivec2 startTile, glm::ivec2 endTile)
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

            //glm::ivec2 lastCellPosition = new(Mathf.RoundToInt(x), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
            glm::ivec2 lastCellPosition(x + 0.5f, y + 0.5f);
            glm::ivec2 cellPosition;
            for (int i = 0; i <= steps; i++)
            {
                //glm::ivec3 cellPosition = new glm::ivec3(Mathf.RoundToInt(x) + (Mathf.RoundToInt(y) % 2), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
                //glm::ivec3 cellPosition = new(Mathf.RoundToInt(x), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
                cellPosition = glm::ivec2(x + 0.5f, y + 0.5f);
                if (cellPosition.y != lastCellPosition.y && cellPosition.x != lastCellPosition.x)
                {
                    if (abs(cellPosition.y % 2) == 0 && cellPosition.x < lastCellPosition.x)
                    {
                        tilemap->SetTile(glm::ivec2(lastCellPosition.x, cellPosition.y), preafabHexagonalTile);
                        //tilemap.SetTile(new glm::ivec3(lastCellPosition.x, cellPosition.y, cellPosition.z), additionalTile);
                        //Debug.Log("Generated additional vertical position: " + new glm::ivec3(lastCellPosition.x, cellPosition.y, cellPosition.z));
                        tilemap->SetTile(glm::ivec2(cellPosition.x, lastCellPosition.y), preafabHexagonalTile);
                        //tilemap.SetTile(new glm::ivec3(cellPosition.x, lastCellPosition.y, cellPosition.z), additionalTile);
                        //Debug.Log("Generated additional sided position: " + new glm::ivec3(cellPosition.x, lastCellPosition.y, cellPosition.z));
                    }
                    else if (abs(cellPosition.y % 2) == 1 && cellPosition.x > lastCellPosition.x)
                    {
                        tilemap->SetTile(glm::ivec2(lastCellPosition.x, cellPosition.y), preafabHexagonalTile);
                        //Debug.Log("Generated additional vertical position: " + new glm::ivec3(lastCellPosition.x, cellPosition.y, cellPosition.z));
                        tilemap->SetTile(glm::ivec2(cellPosition.x, lastCellPosition.y), preafabHexagonalTile);
                        //Debug.Log("Generated additional sided position: " + new glm::ivec3(cellPosition.x, lastCellPosition.y, cellPosition.z));
                    }
                }
                lastCellPosition = cellPosition;
                tilemap->SetTile(cellPosition, preafabHexagonalTile);


                //Debug.Log("Generated position: " + cellPosition);

                x += delta_x;
                y += delta_y;
                //z += delta_z;
            }
        }

        void GenerateRandomHull()
        {
            int pointsNumber = Twin2Engine::Core::Random::Range(minPointsNumber, maxPointsNumber);
            float angleDelta = 2 * glm::pi<float>() / pointsNumber;
            float radiansAngleDeltaRange = glm::radians(angleDeltaRange);
            float angle = 0.0f;

            std::vector<glm::vec2> hexPositions(pointsNumber);

            for (int i = 0; i < pointsNumber; i++)
            {
                float usedAngle = angle + Twin2Engine::Core::Random::Range(-radiansAngleDeltaRange, radiansAngleDeltaRange);

                hexPositions[i] = glm::vec2(glm::cos(usedAngle), glm::sin(usedAngle)) * (generationRadius + Twin2Engine::Core::Random::Range(generationRadiusMinModifier, generationRadiusMaxModifier));

                angle += angleDelta;
            }

            GenerateFloatHull(hexPositions);

            //if (debugMode)
            //{
            //    GenerateForFloatPositions(hexPositions);
            //}
        }
	};
}

#endif // !_MAP_GENERATOR_H_