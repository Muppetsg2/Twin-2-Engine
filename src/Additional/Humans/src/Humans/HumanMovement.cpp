#include <Humans/HumanMovement.h>

using namespace Humans;
using namespace Generation;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

using namespace AStar;

using namespace glm;
using namespace std;

void HumanMovement::Initialize()
{
    //_targetDestination = GetTransform()->GetGlobalPosition();

}
void HumanMovement::OnDestroy()
{
    _notDestroyed = false;

    Component::OnDestroy();

    _findingPath = false;
    _foundPath = false;

    _pathfindingInfo.WaitForFinding();
}
#if TRACY_PROFILER
const char* const tracy_HumanMovementUpdate = "HumanMovementUpdate";
#endif

void HumanMovement::Update()
{
#if TRACY_PROFILER
    ZoneScoped;
    FrameMarkStart(tracy_HumanMovementUpdate);
#endif
    glm::vec3 globalPosition = GetTransform()->GetGlobalPosition();

    globalPosition.y = 0.0f;

    if (_foundPath)
    {
        if (glm::distance(globalPosition, _currentDestination) <= _achievingDestinationAccuracity)
        {
            if (_path.IsOnEnd())
            {
                _findingPath = false;
                _foundPath = false;
            }
            _currentDestination = _path.Next();
            _currentDestination.y = 0.0f;
        }
    }

    //SPDLOG_WARN("Current destination: {} {} {}", _currentDestination.x, _currentDestination.y, _currentDestination.z);

    //vec3 processedDestination = _currentDestination;
    //processedDestination.y = 0;
    glm::vec3 direction = _currentDestination - globalPosition;

    //if (!isnan(direction.x))
    //    SPDLOG_INFO("Not IsNan1:");

    if (glm::dot(direction, direction))
    {
        //SPDLOG_INFO("Chack IsNan1:");
        direction = normalize(direction);
    }

    GetTransform()->Translate(direction * _speed * Time::GetDeltaTime());

    globalPosition = GetTransform()->GetGlobalPosition();


    //if (!isnan(globalPosition.x))
    //    SPDLOG_INFO("Not IsNan3:");

    Tilemap::HexagonalTile* tile = _tilemap->GetTile(
        _tilemap->ConvertToTilemapPosition(
            vec2(globalPosition.x, globalPosition.z) + vec2(direction.x, direction.z) * _forwardDetectionDistance));

    //if (!isnan(globalPosition.x))
    //    SPDLOG_INFO("Not IsNan4:");

    if (tile && tile->GetGameObject())
    {
        GameObject* tileGO = tile->GetGameObject();
        Transform* tileT = tileGO->GetTransform();
        globalPosition.y = tileT->GetGlobalPosition().y;
        GetTransform()->SetGlobalPosition(globalPosition);
        //GetTransform()->SetGlobalPosition(vec3(globalPosition.x, tile->GetGameObject()->GetTransform()->GetGlobalPosition().y, globalPosition.z));
    }

#if TRACY_PROFILER
    FrameMarkEnd(tracy_HumanMovementUpdate);
#endif
}

void HumanMovement::PathFindingSuccess(const AStarPath& path)
{
    if (_notDestroyed)
    {
        _path = path;
        _targetDestination = _bufferedTargetDestination;
        _targetDestination.y = 0.0f;
        _currentDestination = _path.Next();
        _currentDestination.y = 0.0f;
        _foundPath = true;


        //SPDLOG_INFO("PATH_FINDING success");
        //SPDLOG_ERROR("{}Current destination: {} {} {}", GetGameObject()->Id(), _currentDestination.x, _currentDestination.y, _currentDestination.z);
    }
}

void HumanMovement::PathFindingFailure()
{
    _foundPath = false;
    _findingPath = false;

    //SPDLOG_INFO("PATH_FINDING failure");
}

void HumanMovement::SetTilemap(Tilemap::HexagonalTilemap* tilemap)
{
    _tilemap = tilemap;
}

void HumanMovement::MoveTo(glm::vec3 destination)
{
    if (_findingPath || !_notDestroyed)
        return;

    _findingPath = true;

    destination.y = 0.0f;
    _bufferedTargetDestination = destination;

    vec3 globalPosition = GetTransform()->GetGlobalPosition();
    globalPosition.y = 0.0f;

    _pathfindingInfo = AStarPathfinder::FindPath(globalPosition, destination, 0,
        [&](const AStarPath& path) { this->PathFindingSuccess(path); }, [&]() { PathFindingFailure(); });

    _findingPath = _pathfindingInfo.IsSearching();
}

glm::vec3 HumanMovement::GetTargetDestination() const
{
    return _targetDestination;
}


YAML::Node HumanMovement::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "HumanMovement";
    node["speed"] = _speed;
    node["forwardDetectionDistance"] = _forwardDetectionDistance;
    node["achievingDestinationAccuracity"] = _achievingDestinationAccuracity;

    return node;
}

bool HumanMovement::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    _speed = node["speed"].as<float>();
    _forwardDetectionDistance = node["forwardDetectionDistance"].as<float>();
    _achievingDestinationAccuracity = node["achievingDestinationAccuracity"].as<float>();

    return true;
}

#if _DEBUG
void HumanMovement::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Human Movement##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {
        ImGui::InputFloat("Speed: ", &_speed);
        ImGui::InputFloat("ForwardDetectionDistance: ", &_forwardDetectionDistance);
        ImGui::InputFloat("AchievingDestinationAccuracity: ", &_achievingDestinationAccuracity);


        ImGui::Text("Target Destination: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text("%f\t%f\t%f", _targetDestination.x, _targetDestination.y, _targetDestination.z);
        ImGui::PopFont();
        ImGui::Text("Current Destination: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text("%f\t%f\t%f", _currentDestination.x, _currentDestination.y, _currentDestination.z);
        ImGui::PopFont();
    }
}
#endif