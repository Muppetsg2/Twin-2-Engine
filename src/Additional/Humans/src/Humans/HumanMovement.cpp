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

void HumanMovement::Update()
{
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
    glm::vec3 direction = normalize(_currentDestination - globalPosition);

    GetTransform()->Translate(direction * _speed * Time::GetDeltaTime());

    globalPosition = GetTransform()->GetGlobalPosition();

    
    Tilemap::HexagonalTile* tile = _tilemap->GetTile(
        _tilemap->ConvertToTilemapPosition(
            vec2(globalPosition.x, globalPosition.z) + glm::normalize(vec2(direction.x, direction.z)) * _forwardDetectionDistance));
    
    if (tile && tile->GetGameObject())
    {
        GameObject* tileGO = tile->GetGameObject();
        Transform* tileT = tileGO->GetTransform();
        globalPosition.y = tileT->GetGlobalPosition().y;
        GetTransform()->SetGlobalPosition(globalPosition);
        //GetTransform()->SetGlobalPosition(vec3(globalPosition.x, tile->GetGameObject()->GetTransform()->GetGlobalPosition().y, globalPosition.z));
    }
    
}

void HumanMovement::PathFindingSuccess(const AStarPath& path)
{
    _path = path;
    _targetDestination = _bufferedTargetDestination;
    _targetDestination.y = 0.0f;
    _currentDestination = _path.Next();
    _currentDestination.y = 0.0f;
    _foundPath = true;


    SPDLOG_INFO("PATH_FINDING success");
    SPDLOG_ERROR("{}Current destination: {} {} {}", GetGameObject()->Id(), _currentDestination.x, _currentDestination.y, _currentDestination.z);
}

void HumanMovement::PathFindingFailure()
{
    _foundPath = false;
    _findingPath = false;

    SPDLOG_INFO("PATH_FINDING failure");
}

void HumanMovement::SetTilemap(Tilemap::HexagonalTilemap* tilemap)
{
    _tilemap = tilemap;
}

void HumanMovement::MoveTo(glm::vec3 destination)
{
    if (_findingPath)
        return;

    _findingPath = true;

    destination.x = 0.0f;

    vec3 globalPosition = GetTransform()->GetGlobalPosition();
    globalPosition.y = 0.0f;

    AStarPathfinder::FindPath(globalPosition, destination, 0,
        [&](const AStarPath& path) { this->PathFindingSuccess(path); }, [&]() { PathFindingFailure(); });

    _bufferedTargetDestination = destination;
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