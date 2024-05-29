#include <EnemyMovement.h>

#include <physic/CollisionManager.h>
#include <physic/Ray.h>
#include <core/CameraComponent.h>
#include <core/Input.h>
#include <core/Time.h>

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
    if (_path) {
        delete _path;
        _path = nullptr;
    }
    _info.WaitForFinding();
}

void EnemyMovement::Update() {
    Transform* transform = GetTransform();
    glm::vec3 position = transform->GetGlobalPosition();
   if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive)
    {

    }
    else
    {
        //lineRenderer->positionCount = 0;
        //circleRenderer->positionCount = 0;
    }

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
            //transform->SetGlobalPosition(_waypoint + vec3(0.0f, 0.5f, 0.0f)); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
            transform->SetGlobalPosition(_waypoint); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
            _waypoint = _path->Next();
            _waypoint.y += _heightOverSurface;
        }
        else {
            //transform->SetGlobalPosition(glm::vec3(glm::mix(position, tempWaypointPos, 0.5f)) + vec3(0.0f, 0.5f, 0.0f));
            float walk_dist = Time::GetDeltaTime() * speed;
            transform->Translate(glm::normalize(_waypoint - position) * walk_dist);
        }
    }
}


void EnemyMovement::OnPathComplete(const AStarPath& p) {

    if (_path) {
        delete _path;
        _path = nullptr;
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

void EnemyMovement::OnPathFailure() {
    EndMoveAction();
    OnFindPathError(GetGameObject(), tempDestTile);
}

void EnemyMovement::EndMoveAction() {
    reachEnd = true;
    //alreadyChecked = false;
    if (_path) {
        delete _path;
        _path = nullptr;
    }
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
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool EnemyMovement::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

#if _DEBUG
void EnemyMovement::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("EnemyMovement##Component").append(id);
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