#include <PlayerMovement.h>
#include <physic/CollisionManager.h>
#include <physic/Ray.h>
#include <core/CameraComponent.h>
#include <core/Input.h>
#include <core/Time.h>
#include <cmath>
#include <string>

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
}

void PlayerMovement::OnDestroy() {
    //seeker = GetGameObject()->GetComponent<Seeker>();
    if (_path) {
        delete _path;
    }
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

    if (Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::LEFT) && reachEnd && !GameManager::instance->minigameActive)
    {
        if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        {
            SPDLOG_INFO("COL_ID: {}", raycastHit.collider->colliderId);
            HexTile* hexTile = raycastHit.collider->GetGameObject()->GetComponent<HexTile>();
            MapHexTile* mapHexTile = hexTile->GetMapHexTile();
            //if (raycastHit.transform != null && raycastHit.transform.gameObject.TryGetComponent(out HexTile hexTile) && hexTile.Type != TileType.Mountain && !(hexTile.Type == TileType.RadioStation && hexTile.currCooldown > 0f) && !hexTile.IsFighting)
            if (mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation && hexTile->currCooldown > 0.0f) && !hexTile->IsFighting)
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

    //if (path == nullptr)
    //    return;
    //
    //if (currWaypoint >= path->vectorPath.Count /*|| currWaypoint >= actualEnd*/)
    //{
    //    transform->SetGlobalPosition(glm::vec3((path->vectorPath[currWaypoint - 1].x, position.y, path->vectorPath[currWaypoint - 1].z)));
    //    EndMoveAction();
    //    OnFinishMoving.Invoke(GetGameObject(), destinatedTile);
    //    destinatedTile = nullptr;
    //    return;
    //}
    //else
    //{
    //    reachEnd = false;
    //}
    
    //glm::vec3 waypoint(path->vectorPath[currWaypoint].x, position.y, path->vectorPath[currWaypoint].z);
    //glm::vec3 waypoint(destination.x, position.y, destination.z);

    /*
    // Sprawdzenie 1 do przodu
    if (currWaypoint + 1 < path.vectorPath.Count && !alreadyChecked)
    {
        Vector3 f1 = new(path.vectorPath[currWaypoint + 1].x, transform.position.y, path.vectorPath[currWaypoint + 1].z);

        HexTile t1 = FindTile(f1, TileType.Mountain | TileType.Water);

        if (t1 != null)
        {
            if (t1.Type == TileType.Mountain || t1.Type == TileType.Water)
            {
                destinatedTile = FindTile(waypoint, TileType.Empty | TileType.PointOfInterest | TileType.RadioStation);
                actualEnd = currWaypoint + 1;
                ++currWaypoint;
                return;
            }
        }
    }

    // Sprawdzenie 2 do przodu
    if (currWaypoint + 2 < path.vectorPath.Count)
    {
        Vector3 f2 = new(path.vectorPath[currWaypoint + 2].x, transform.position.y, path.vectorPath[currWaypoint + 2].z);

        HexTile t2 = FindTile(f2, TileType.Water | TileType.Mountain);

        if (t2 == null || t2.Type == TileType.Mountain || t2.Type == TileType.Water)
        {
            destinatedTile = FindTile(new(path.vectorPath[currWaypoint + 1].x, transform.position.y, path.vectorPath[currWaypoint + 1].z), TileType.Empty | TileType.PointOfInterest | TileType.RadioStation);
            actualEnd = currWaypoint + 2;
            alreadyChecked = true;
        }
    }
    */

    /*
    if (currWaypoint + 1 == actualEnd || currWaypoint == 0)
    {
        transform.position = Vector3.Lerp(transform.position, waypoint, Time.deltaTime * speed);
    }
    else
    {
        transform.position = Vector3.MoveTowards(transform.position, waypoint, Time.deltaTime * speed);
    }
    */

    if (!reachEnd) {
        position = transform->GetGlobalPosition();

        vec3 directionPos = position;
        directionPos.y = 0.0f;

        //vec3 tempWaypointPos = _waypoint;
        //tempWaypointPos.y = tile->GetGameObject()->GetTransform()->GetGlobalPosition().y + 0.5f;
        //tempWaypointPos.y += 0.5f;

        float dist = glm::distance(position, _waypoint);

        Tilemap::HexagonalTile* tile = _tilemap->GetTile(_tilemap->ConvertToTilemapPosition(vec2(_waypoint.x, _waypoint.z)));

        if (dist <= nextWaypointDistance) {
            if (_path->IsOnEnd())
            {
                reachEnd = true;
            }
            //transform->SetGlobalPosition(_waypoint + vec3(0.0f, 0.5f, 0.0f)); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
            transform->SetGlobalPosition(_waypoint); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
            _waypoint = _path->Next();
            _waypoint.y += 0.5f;
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
    }

    _path = new AStarPath(p);

    destination = tempDest;
    destinatedTile = tempDestTile;
    _waypoint = _path->Next();
    _waypoint.y += 0.5f;

    reachEnd = false;

    OnStartMoving.Invoke(GetGameObject(), destinatedTile);


    tempDestTile = nullptr;
}

void PlayerMovement::OnPathFailure() {
    EndMoveAction();
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
        if (mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation && dest->currCooldown > 0.0f) && !dest->IsFighting)
        {
            SetDestination(dest);
        }
    }
}



bool PlayerMovement::InCircle(glm::vec3 point) {
    glm::vec3 position = GetTransform()->GetGlobalPosition();
    return (point.x - position.x) * (point.x - position.x) + (point.z - position.z) * (point.z - position.z) < radius * radius;
}

void PlayerMovement::SetDestination(HexTile* dest) {
    glm::vec3 destPos = dest->GetTransform()->GetGlobalPosition() + glm::vec3(0.0f, 0.5f, 0.0f);
    if (!GameManager::instance->gameStarted)
    {
        GetTransform()->SetGlobalPosition(destPos);
        destinatedTile = dest;
        destination = destPos;
        GameManager::instance->gameStarted = true;
        OnStartMoving.Invoke(GetGameObject(), dest);
    }
    else if (InCircle(destPos))
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
                AStarPathfinder::FindPath(GetTransform()->GetGlobalPosition(), dest->GetTransform()->GetGlobalPosition(), maxSteps,
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
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool PlayerMovement::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) 
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

#if _DEBUG
void PlayerMovement::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("PlayerMovement##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        //glm::vec3 v = light->direction;
        //ImGui::DragFloat3(string("Direction##").append(id).c_str(), glm::value_ptr(v), .1f, -1.f, 1.f);
        //if (v != light->direction) {
        //    SetDirection(v);
        //
        //float p = light->power;
        //ImGui::DragFloat(string("Power##").append(id).c_str(), &p);
        //if (p != light->power) {
        //    SetPower(p);
        //}
    }
}
#endif