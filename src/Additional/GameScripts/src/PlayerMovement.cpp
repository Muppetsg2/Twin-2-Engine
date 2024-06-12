#include "PlayerMovement.h"
#include <PlayerMovement.h>
#include <physic/CollisionManager.h>
#include <physic/Ray.h>
#include <core/CameraComponent.h>
#include <core/Input.h>
#include <core/Time.h>
#include <cmath>
#include <string>
#include <UIScripts/MinigameManager.h>

// PATH FINDING
#include <AstarPathfinding/AStarPath.h>
#include <AstarPathfinding/AStarPathfinder.h>

using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace Generation;
using namespace AStar;

void PlayerMovement::Initialize() {
	//seeker = GetGameObject()->GetComponent<Seeker>();
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();

    //OnFinishMoving.AddCallback([](GameObject* playerGO, HexTile* destHexTile) {
    //    Playable* occupyingEntity = destHexTile->occupyingEntity;
    //    if (occupyingEntity != nullptr) {
    //        MinigameManager::GetLastInstance()->StartMinigame(playerGO->GetComponent<Player>(), occupyingEntity);
    //    }
    //    });
}

void PlayerMovement::OnDestroy() {
    //seeker = GetGameObject()->GetComponent<Seeker>();
    if (_path) {
        delete _path;
        _path = nullptr;
    }
    _info.WaitForFinding();
}

void PlayerMovement::Update() {
    Ray ray = CameraComponent::GetMainCamera()->GetScreenPointRay(Input::GetCursorPos());// Camera.main.ScreenPointToRay(Input.mousePosition);
    RaycastHit raycastHit;

    Transform* transform = GetTransform();
    glm::vec3 position = transform->GetGlobalPosition();

    if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive)
    {
        DrawCircle(steps, radius);

        if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        {
            if (InCircle(raycastHit.position))
            {
                DrawLine(position, glm::vec3(raycastHit.position.x, position.y, raycastHit.position.z));
            }
            else
            {
                //lineRenderer->positionCount = 0;
            }
        }
        else
        {
            //lineRenderer->positionCount = 0;
        }
    }
    else
    {
        //lineRenderer->positionCount = 0;
        //circleRenderer->positionCount = 0;
    }

    if (GameManager::instance->gameStarted && Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::LEFT) && reachEnd && !GameManager::instance->minigameActive)
    {
        if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        {
            SPDLOG_INFO("COL_ID: {}", raycastHit.collider->colliderId);
            HexTile* hexTile = raycastHit.collider->GetGameObject()->GetComponent<HexTile>();
            MapHexTile* mapHexTile = hexTile->GetMapHexTile();
            //if (raycastHit.transform != null && raycastHit.transform.gameObject.TryGetComponent(out HexTile hexTile) && hexTile.Type != TileType.Mountain && !(hexTile.Type == TileType.RadioStation && hexTile.currCooldown > 0f) && !hexTile.IsFighting)
            SPDLOG_INFO("Player destination HexTileType: {}", hexTile->isFighting);
            if (mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation && hexTile->currCooldown > 0.0f) && !hexTile->isFighting)
            {
                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //if (!EventSystem.current.IsPointerOverGameObject())
                {
                    /*
                    if (!GameManager.instance.gameStarted)
                    {
                        transform.position = hexTile.sterowiecPos;
                        OnStartMoving?.Invoke(gameObject, hexTile);
                        destinatedTile = hexTile;
                    }
                    else if (InCircle(raycastHit.transform.position))
                    {
                        if (seeker.IsDone())
                        {
                            tempDest = raycastHit.transform.position;
                            tempDest.y = hexTile.sterowiecPos.y;
                            seeker.StartPath(transform.position, tempDest, OnPathComplete);
                            tempDestTile = hexTile;
                        }
                    }
                    */

                    SetDestination(hexTile);
                    position = transform->GetGlobalPosition();
                }
            }
            //else if (GameManager::instance->gameStarted && raycastHit.transform != null && raycastHit.transform.gameObject.TryGetComponent(out HexTile t2))
            else if (GameManager::instance->gameStarted)
            {
                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //HUDInfo obj = FindObjectOfType<HUDInfo>();
                //if (obj != null)
                //{
                //    obj.SetInfo("You cannot enter this field.", 1.5f);
                //}
            }
        }
    }

    if (!reachEnd) {
        position = transform->GetGlobalPosition();

        float dist = glm::distance(position, _waypoint);

        Tilemap::HexagonalTile* tile = _tilemap->GetTile(_tilemap->ConvertToTilemapPosition(vec2(_waypoint.x, _waypoint.z)));

        if (dist <= nextWaypointDistance) {
            if (_path->IsOnEnd())
            {
                reachEnd = true;
                SPDLOG_INFO("On end of path");
                OnFinishMoving(GetGameObject(), destinatedTile);
            }
            else
            {
                position = _waypoint;
                transform->SetGlobalPosition(position);
                _waypoint = _path->Next();
                _waypoint.y += _heightOverSurface;

                vec3 direction = _waypoint - position;
                direction.y = 0.0f;
                float angle = glm::degrees(glm::acos(direction.x / glm::length(direction)));
                if (direction.z > 0.0f)
                {
                    angle = -angle;
                }
                GetTransform()->SetGlobalRotation(glm::vec3(0.0f, angle + 90, 0.0f));
            }
        }
        else {
            //transform->SetGlobalPosition(glm::vec3(glm::mix(position, tempWaypointPos, 0.5f)) + vec3(0.0f, 0.5f, 0.0f));
            float walk_dist = Time::GetDeltaTime() * speed;
            transform->Translate(glm::normalize(_waypoint - position) * walk_dist);
        }
    }
}   


void PlayerMovement::OnPathComplete(const AStarPath& p) {

    if (_path) {
        delete _path;
        _path = nullptr;
    }

    _path = new AStarPath(p);

    destination = tempDest;
    destinatedTile = tempDestTile;
    _waypoint = _path->Next();
    _waypoint.y += _heightOverSurface;

    reachEnd = false;

    OnStartMoving.Invoke(GetGameObject(), destinatedTile);


    tempDestTile = nullptr;
}

void PlayerMovement::OnPathFailure() {
    EndMoveAction();
    OnFindPathError(GetGameObject(), tempDestTile);
}

void PlayerMovement::EndMoveAction() {
    reachEnd = true;
    //alreadyChecked = false;
    if (_path) {
        delete _path;
        _path = nullptr;
    }
}

void PlayerMovement::MoveAndSetDestination(HexTile* dest) {
    if (reachEnd && !GameManager::instance->minigameActive)
    {
        MapHexTile* mapHexTile = dest->GetMapHexTile();
        if (mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation && dest->currCooldown > 0.0f) && !dest->isFighting)
        {
            SetDestination(dest);
        }
    }
}

void PlayerMovement::StartUp(HexTile* startUpTile)
{
    vec3 startUpPosition = startUpTile->GetTransform()->GetGlobalPosition();
    GetTransform()->SetGlobalPosition(startUpPosition);
    //destinatedTile = dest;
    //destination = destPos;
    GameManager::instance->gameStarted = true;
    OnStartMoving.Invoke(GetGameObject(), startUpTile);
    //Player* player = GetGameObject()->GetComponent<Player>();
    //player->CurrTile = dest;
    //dest->StartTakingOver(player);
    OnFinishMoving(GetGameObject(), startUpTile);
}

bool PlayerMovement::InCircle(glm::vec3 point) {
    glm::vec3 position = GetTransform()->GetGlobalPosition();
    return (point.x - position.x) * (point.x - position.x) + (point.z - position.z) * (point.z - position.z) < radius * radius;
}

void PlayerMovement::SetDestination(HexTile* dest) {
    glm::vec3 destPos = dest->GetTransform()->GetGlobalPosition() + glm::vec3(0.0f, _heightOverSurface, 0.0f);
    
    if (InCircle(destPos))
    {
        if (!_path || _path->IsOnEnd())
        {
            tempDestTile = dest;

            //if (_info)
            //{
            //    delete _info;
            //}
            //
            //_info = new AStarPathfindingInfo(AStarPathfinder::FindPath(GetTransform()->GetGlobalPosition(), dest->GetTransform()->GetGlobalPosition(), maxSteps,
            //    [&](const AStarPath& path) { OnPathComplete(path); }, [&]() { OnPathFailure(); }));
            _info = AStarPathfinder::FindPath(GetTransform()->GetGlobalPosition(), dest->GetTransform()->GetGlobalPosition(), maxSteps,
                    [&](const AStarPath& path) { OnPathComplete(path); }, [&]() { OnPathFailure(); });

        }
    }
}

void PlayerMovement::DrawCircle(int steps, float radius) {
    //circleRenderer->positionCount = steps;

    for (int i = 0; i < steps; ++i)
    {
        float p = (float)i / steps;
        float radian = p * 2 * M_PI;

        float x = glm::cos(radian) * radius;
        float z = glm::sin(radian) * radius;

        glm::vec3 position = GetTransform()->GetGlobalPosition();

        //circleRenderer->SetPosition(i, glm::vec3(position.x + x, position.y, position.z + z));
    }
}

void PlayerMovement::DrawLine(glm::vec3 startPos, glm::vec3 endPos) {
    //lineRenderer->positionCount = 2;
    //lineRenderer->SetPosition(0, startPos);
    //lineRenderer->SetPosition(1, endPos);
}

YAML::Node PlayerMovement::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "PlayerMovement";
    node["speed"] = speed;
    node["maxSteps"] = maxSteps;
    node["nextWaypointDistance"] = nextWaypointDistance;
    node["heightOverSurface"] = _heightOverSurface;

    return node;
}

bool PlayerMovement::Deserialize(const YAML::Node& node)
{
    if (!node["speed"] || !node["maxSteps"] || !node["nextWaypointDistance"] ||
        !node["heightOverSurface"] || !Component::Deserialize(node))
        return false;

    speed = node["speed"].as<float>();
    maxSteps = node["maxSteps"].as<size_t>();
    nextWaypointDistance = node["nextWaypointDistance"].as<float>();
    _heightOverSurface = node["heightOverSurface"].as<float>();

    return true;
}

#if _DEBUG
void PlayerMovement::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Player Movement##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}
#endif