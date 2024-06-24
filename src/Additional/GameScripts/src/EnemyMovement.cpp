#include <EnemyMovement.h>

#include <physic/CollisionManager.h>
#include <physic/Ray.h>
#include <core/CameraComponent.h>
#include <core/Input.h>
#include <core/Time.h>

#include <UIScripts/MinigameManager.h>

// PATH FINDING
#include <AstarPathfinding/AStarPath.h>
#include <AstarPathfinding/AStarPathfinder.h>

using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace Generation;
using namespace AStar;

void EnemyMovement::Initialize() {
    //seeker = GetGameObject()->GetComponent<Seeker>();
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
}

void EnemyMovement::OnDestroy() {
    //seeker = GetGameObject()->GetComponent<Seeker>();
    _info.WaitForFinding();
    _mutexPath.lock();
    if (_path) {
        delete _path;
        _path = nullptr;
    }
    _mutexPath.unlock();
}

void EnemyMovement::Update() {

    if (_startMovingAgument)
    {
        OnStartMoving.Invoke(GetGameObject(), _startMovingAgument);
        _startMovingAgument = nullptr;
    }

    Transform* transform = GetTransform();
    glm::vec3 position = transform->GetGlobalPosition();
    if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive)
    {
        if (!reachEnd) {
            position = transform->GetGlobalPosition();

            float dist = glm::distance(position, _waypoint);

            Tilemap::HexagonalTile* tile = _tilemap->GetTile(_tilemap->ConvertToTilemapPosition(vec2(_waypoint.x, _waypoint.z)));

            if (dist <= nextWaypointDistance) {
                if (_path->IsOnEnd())
                {
                    reachEnd = true;
                    OnFinishMoving(GetGameObject(), destinatedTile);
                }
                else
                {
                    //transform->SetGlobalPosition(_waypoint + vec3(0.0f, 0.5f, 0.0f)); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
                    transform->SetGlobalPosition(_waypoint); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
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
    else
    {
        //lineRenderer->positionCount = 0;
        //circleRenderer->positionCount = 0;
    }

}


void EnemyMovement::OnPathComplete(const AStarPath& p) {
    _mutexPath.lock();
    if (_path) {
        delete _path;
        _path = nullptr;
    }

    _path = new AStarPath(p);
    
    destination = tempDest;
    destinatedTile = tempDestTile;
    _waypoint = _path->Next();
    //_waypoint.y += 0.5f;

    reachEnd = false;

    _startMovingAgument = destinatedTile;
    //OnStartMoving.Invoke(GetGameObject(), destinatedTile);

    tempDestTile = nullptr;

    _mutexPath.unlock();
}

void EnemyMovement::OnPathFailure() {
    EndMoveAction();
    OnFindPathError(GetGameObject(), tempDestTile);
}

void EnemyMovement::EndMoveAction() {
    _mutexPath.lock();
    reachEnd = true;
    //alreadyChecked = false;
    if (_path) {
        delete _path;
        _path = nullptr;
    }
    _mutexPath.unlock();
}

bool EnemyMovement::InCircle(glm::vec3 point) {
    glm::vec3 position = GetTransform()->GetGlobalPosition();
    return (point.x - position.x) * (point.x - position.x) + (point.z - position.z) * (point.z - position.z) < radius * radius;
}

void EnemyMovement::SetDestination(HexTile* dest) {
    glm::vec3 destPos = dest->GetTransform()->GetGlobalPosition() + glm::vec3(0.0f, _heightOverSurface, 0.0f);
    //if (!GameManager::instance->gameStarted)
    //{
    //    GetTransform()->SetGlobalPosition(destPos);
    //    destinatedTile = dest;
    //    destination = destPos;
    //    GameManager::instance->gameStarted = true;
    //    OnStartMoving.Invoke(GetGameObject(), dest);
    //}
    //else 
    //if (InCircle(destPos))
    {
        if (!_path || _path->IsOnEnd())
        {
            tempDestTile = dest;
            _info = AStarPathfinder::FindPath(GetTransform()->GetGlobalPosition(), dest->GetTransform()->GetGlobalPosition(), maxSteps,
                [&](const AStarPath& path) { OnPathComplete(path); }, [&]() { OnPathFailure(); });

        }
    }
}

YAML::Node EnemyMovement::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "EnemyMovement";
    node["speed"] = speed;
    node["maxSteps"] = maxSteps;
    node["nextWaypointDistance"] = nextWaypointDistance;
    node["heightOverSurface"] = _heightOverSurface;

    return node;
}

bool EnemyMovement::Deserialize(const YAML::Node& node)
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
void EnemyMovement::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Enemy Movement##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}
#endif