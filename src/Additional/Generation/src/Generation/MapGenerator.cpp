#include <Generation/MapGenerator.h>
#include <core/Time.h>
#include <Generation/ContentGenerator.h>
#include <core/HexagonalColliderComponent.h>

using namespace Generation;
using namespace Tilemap;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace std;
using namespace glm;

void MapGenerator::GenerateFloatHull(const vector<vec2>& hull)
{
    vector<ivec2> hullInt(hull.size());

    for (int i = 0; i < hull.size(); i++)
    {
        //hullInt[i] = ivec2(hull[i].x + 0.5f, hull[i].y + 0.5f);
        hullInt[i] = tilemap->ConvertToTilemapPosition(hull[i]);
    }

    for (int i = 0; i < (hullInt.size() - 1); i++)
    {
        ConnectTiles(hullInt[i], hullInt[i + 1]);
    }
    ConnectTiles(hullInt[hullInt.size() - 1], hullInt[0]);
}


void MapGenerator::GeneratePositions(const vector<ivec2>& positions)
{
    for (int i = 0; i < positions.size(); i++)
    {
        tilemap->SetTile(positions[i], preafabHexagonalTile);
        //tilemap->SetTile(positions[i], pointTile);
    }
}

void MapGenerator::GenerateForFloatPositions(const vector<vec2>& positions)
{
    vector<ivec2> temp(positions.size());

    for (int i = 0; i < positions.size(); i++)
    {
        //temp[i] = ivec2(positions[i].x + 0.5f, positions[i].y + 0.5f);
        temp[i] = tilemap->ConvertToTilemapPosition(positions[i]);
    }


    GeneratePositions(temp);
}

void MapGenerator::ConnectTiles(ivec2 startTile, ivec2 endTile)
{
#if HEX_DIRECTION_CONNECTING
    ivec2 cellPosition(startTile);
    vec2 currentPos = tilemap->ConvertToRealPosition(cellPosition);
    vec2 destPosition = tilemap->ConvertToRealPosition(endTile);

    const vec2 normalDirections[6]
    {
        vec2(-0.866025, 0.5), vec2(0, 1), vec2(0.866025, 0.5),
        vec2(0.866025, -0.5), vec2(0, -1), vec2(-0.866025, -0.5)
    };

    //for (int i = 0; i <= steps; i++)
    while (cellPosition != endTile)
    {
        vec2 direction = destPosition - currentPos;
        tilemap->SetTile(cellPosition, preafabHexagonalTile);
        if (destPosition.y <= currentPos.y)
        {
            if (destPosition.x <= currentPos.x)
            {
                if (glm::dot(direction, normalDirections[5]) >= glm::dot(direction, normalDirections[4]))
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 5);
                }
                else
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 4);
                }
            }
            else
            {
                if (glm::dot(direction, normalDirections[3]) >= glm::dot(direction, normalDirections[4]))
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 3);
                }
                else
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 4);
                }
            }
        }
        else
        {
            if (destPosition.x <= currentPos.x)
            {
                if (glm::dot(direction, normalDirections[0]) >= glm::dot(direction, normalDirections[1]))
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 0);
                }
                else
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 1);
                }
            }
            else
            {
                if (glm::dot(direction, normalDirections[2]) >= glm::dot(direction, normalDirections[1]))
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 2);
                }
                else
                {
                    cellPosition = tilemap->GetPositionInDirection(cellPosition, 1);
                }
            }
        }
        currentPos = tilemap->ConvertToRealPosition(cellPosition);
    }
#elif GRADIENT_CONNECTING
    ///////////////////////////////////
    int dx = endTile.x - startTile.x;
    int dy = endTile.y - startTile.y;
    //int dz = endTile.z - startTile.z;
    
    int steps = max(abs(dx), abs(dy)); // Zastanowi� si� czy to jako� nie zmieni� z makr na funckje
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
    ivec2 cellPosition(startTile);

    for (int i = 0; i <= steps; i++)
    //while (cellPosition != endTile)
    {
        //ivec3 cellPosition = new ivec3(Mathf.RoundToInt(x) + (Mathf.RoundToInt(y) % 2), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
        //ivec3 cellPosition = new(Mathf.RoundToInt(x), Mathf.RoundToInt(y), Mathf.RoundToInt(z));
        cellPosition = ivec2(x + 0.5f, y + 0.5f);
        if (cellPosition.y != lastCellPosition.y && cellPosition.x != lastCellPosition.x)
        {
            if (abs(cellPosition.y % 2) == 0 && cellPosition.x < lastCellPosition.x)
            {
                tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), preafabHexagonalTile);
                //tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), additionalTile);
        
                tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), preafabHexagonalTile);
                //tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), additionalTile);
        
            }
            else if (abs(cellPosition.y % 2) == 1 && cellPosition.x > lastCellPosition.x)
            {
                tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), preafabHexagonalTile);
                //tilemap->SetTile(ivec2(lastCellPosition.x, cellPosition.y), additionalTile);
        
                tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), preafabHexagonalTile);
                //tilemap->SetTile(ivec2(cellPosition.x, lastCellPosition.y), additionalTile);
            }
        }
        lastCellPosition = cellPosition;
        tilemap->SetTile(cellPosition, preafabHexagonalTile);

        x += delta_x;
        y += delta_y;
        //z += delta_z;

    }
#endif
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

        hexPositions[i] = vec2(cos(usedAngle), sin(usedAngle)) * (Twin2Engine::Core::Random::Range(generationRadiusMin, generationRadiusMax));

        angle += angleDelta;
    }

    GenerateFloatHull(hexPositions);

    //if (debugMode)
    //{
    //    GenerateForFloatPositions(hexPositions);
    //}
    //GenerateForFloatPositions(hexPositions);
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
    //tilemap->Fill(ivec2(0, 0), filledTile);
    tilemap->Fill(ivec2(0, 0), preafabHexagonalTile);

    //tilemapRegionDivider = GetComponent<ITilemapRegionDivider>();
    //tilemapRegionDivider.DivideTilemap();


    //BoundsInt bounds = tilemap.cellBounds;
    ivec2 leftBottomPosition = tilemap->GetLeftBottomPosition();
    ivec2 rightTopPosition = tilemap->GetRightTopPosition();
    
    int i = 1000;
    for (int x = leftBottomPosition.x; x <= rightTopPosition.x; x++)
    {
        for (int y = leftBottomPosition.y; y <= rightTopPosition.y; y++)
        {
            ivec2 tilePosition(x, y);
            HexagonalTile* tile = tilemap->GetTile(tilePosition);
            GameObject* tileObject = tile->GetGameObject();
            if (tileObject != nullptr)
            {
                MapHexTile* hexTile = tileObject->GetComponent<MapHexTile>();
                hexTile->tilemap = tilemap;
                hexTile->tile = tile;

                HexagonalColliderComponent* hexCol = tileObject->GetComponent<HexagonalColliderComponent>();
                if (hexCol != nullptr) {
                    hexCol->colliderId = ++i;
                    auto Pos = hexCol->GetTransform()->GetGlobalPosition();
                    
                    //SPDLOG_INFO("Pos: {}\t{}\t{}\t\t\t\tColPos: {}\t{}\t{}", Pos.x, Pos.y, Pos.z, hexCol->collider->shapeColliderData->Position.x, Pos.z, hexCol->collider->shapeColliderData->Position.y, Pos.z, hexCol->collider->shapeColliderData->Position.z);
                }
            }
        }
    }
    
    //GetComponent<IContentGenerator>() ? .GenerateContent();
}

YAML::Node MapGenerator::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "MapGenerator";
    node["prefabHexagonalTile"] = SceneManager::GetPrefabSaveIdx(preafabHexagonalTile->GetId());
    node["additionalTile"] = SceneManager::GetPrefabSaveIdx(additionalTile->GetId());
    node["filledTile"] = SceneManager::GetPrefabSaveIdx(filledTile->GetId());
    node["pointTile"] = SceneManager::GetPrefabSaveIdx(pointTile->GetId());
    node["generationRadiusMin"] = generationRadiusMin;
    node["generationRadiusMax"] = generationRadiusMax;
    node["minPointsNumber"] = minPointsNumber;
    node["maxPointsNumber"] = maxPointsNumber;
    node["angleDeltaRange"] = angleDeltaRange;
    return node;
}

bool MapGenerator::Deserialize(const YAML::Node& node) {
    if (!node["prefabHexagonalTile"] || !node["additionalTile"] || !node["filledTile"] ||
        !node["pointTile"] || !node["generationRadiusMin"] || !node["generationRadiusMax"] ||
        !node["minPointsNumber"] || !node["maxPointsNumber"] || !node["angleDeltaRange"] ||
        !Component::Deserialize(node)) return false;

    preafabHexagonalTile = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["prefabHexagonalTile"].as<size_t>()));
    additionalTile = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["additionalTile"].as<size_t>()));
    filledTile = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["filledTile"].as<size_t>()));
    pointTile = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["pointTile"].as<size_t>()));

    generationRadiusMin = node["generationRadiusMin"].as<float>();
    generationRadiusMax = node["generationRadiusMax"].as<float>();
    minPointsNumber = node["minPointsNumber"].as<int>();
    maxPointsNumber = node["maxPointsNumber"].as<int>();
    angleDeltaRange = node["angleDeltaRange"].as<float>();

    return true;
}

void MapGenerator::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("Map Generator##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str()))
    {
        Component::DrawInheritedFields();

        // TODO: DODAC
        /*
        node["preafabHexagonalTile"] = PrefabManager::GetPrefabPath(preafabHexagonalTile);
        node["additionalTile"] = PrefabManager::GetPrefabPath(additionalTile);
        node["filledTile"] = PrefabManager::GetPrefabPath(filledTile);
        node["pointTile"] = PrefabManager::GetPrefabPath(pointTile);
        */

        float v = generationRadiusMin;
        ImGui::DragFloat(string("Generation Radius Min##").append(id).c_str(), &v, 0.1f);

        if (v != generationRadiusMin) {
            generationRadiusMin = v;
        }

        v = generationRadiusMax;
        ImGui::DragFloat(string("Generation Radius Max##").append(id).c_str(), &v, 0.1f);

        if (v != generationRadiusMax) {
            generationRadiusMax = v;
        }

        v = minPointsNumber;
        ImGui::DragFloat(string("Points Number Min##").append(id).c_str(), &v, 0.1f);

        if (v != minPointsNumber) {
            minPointsNumber = v;
        }

        v = maxPointsNumber;
        ImGui::DragFloat(string("Points Number Max##").append(id).c_str(), &v, 0.1f);

        if (v != maxPointsNumber) {
            maxPointsNumber = v;
        }

        v = angleDeltaRange;
        ImGui::DragFloat(string("Delta Range Angle##").append(id).c_str(), &v, 0.1f);

        if (v != angleDeltaRange) {
            angleDeltaRange = v;
        }

        if (ImGui::Button(string("Generate##").append(id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
        {
            Transform* tilemapTransform = tilemap->GetTransform();
            tilemap->Clear();
            while (tilemapTransform->GetChildCount())
            {
                Transform* child = tilemapTransform->GetChildAt(0);
                tilemapTransform->RemoveChild(child);
                SceneManager::DestroyGameObject(child->GetGameObject());
            }

            float tilemapGenerating = Time::GetTime();
            Generate();
            spdlog::info("Tilemap generation: {}ms", Time::GetTime() - tilemapGenerating);

            ContentGenerator* contentGenerator = this->GetGameObject()->GetComponent<ContentGenerator>();
            if (contentGenerator != nullptr) {
                tilemapGenerating = Time::GetTime();
                contentGenerator->GenerateContent(tilemap);
                spdlog::info("Tilemap content generation: {}", Time::GetTime() - tilemapGenerating);
            }
            else {
                spdlog::info("Map Generator: Couldn't find Content Generator");
            }
        }
    }
}