#include <Humans/HumanMovement.h>

using namespace Humans;
using namespace Generation;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

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
    glm::vec3 direction = normalize(_targetDestination - globalPosition);

    GetTransform()->Translate(direction * _speed * Time::GetDeltaTime());

    globalPosition = GetTransform()->GetGlobalPosition();

    
    Tilemap::HexagonalTile* tile = _tilemap->GetTile(
        _tilemap->ConvertToTilemapPosition(
            vec2(globalPosition.x, globalPosition.z) + vec2(direction.x, direction.z) * _forwardDetectionDistance));
    
    if (tile && tile->GetGameObject())
    {
        GameObject* tileGO = tile->GetGameObject();
        Transform* tileT = tileGO->GetTransform();
        globalPosition.y = tileT->GetGlobalPosition().y;
        //GetTransform()->SetGlobalPosition(vec3(globalPosition.x, tile->GetGameObject()->GetTransform()->GetGlobalPosition().y, globalPosition.z));
        GetTransform()->SetGlobalPosition(globalPosition);
    }

    //RaycastHit forawrdHit; // = Physics.RaycastAll(transform.position + new Vector3(0, 1, 0) + direction * _forwardDetectionDistance, Vector3.down, 5f);
    //RaycastHit currentHit; // = Physics.RaycastAll(transform.position + new Vector3(0, 1, 0), Vector3.down, 5f);
    //
    //Ray forwardRay(vec3(0.0f, -1.0f, 0.0f), globalPosition + vec3(0, 2, 0) + direction * _forwardDetectionDistance);
    //Ray currentRay(vec3(0.0f, -1.0f, 0.0f), globalPosition + vec3(0, 0.1, 0));
    //
    //bool fHit = CollisionManager::Instance()->Raycast(forwardRay, forawrdHit);
    //bool cHit = CollisionManager::Instance()->Raycast(currentRay, currentHit);

    ////Debug.Log("Hits: " + forawrdHit.Length);
    //MapHexTile* forwardHexTile = nullptr;
    ////SPDLOG_INFO("Found frwdRay: {}", forawrdHit.collider != nullptr);
    //if (fHit)
    //{
    //    forwardHexTile = forawrdHit.collider->GetGameObject()->GetComponent<MapHexTile>();
    //}
    //
    //MapHexTile* currentHexTile = nullptr;
    //if (cHit)
    //{
    //    currentHexTile = currentHit.collider->GetGameObject()->GetComponent<MapHexTile>();
    //}
    //
    ////if (currentHexTile != nullptr && forwardHexTile != nullptr)
    //if (fHit && cHit)
    //{
    //    float currentPositionY = currentHexTile->GetTransform()->GetGlobalPosition().y;
    //    float forwardPositionY = forwardHexTile->GetTransform()->GetGlobalPosition().y;
    //    if (currentPositionY > forwardPositionY)
    //    {
    //        GetTransform()->SetGlobalPosition(vec3(GetTransform()->GetGlobalPosition().x, currentPositionY, GetTransform()->GetGlobalPosition().z));
    //    }
    //    else
    //    {
    //        GetTransform()->SetGlobalPosition(vec3(GetTransform()->GetGlobalPosition().x, forwardPositionY, GetTransform()->GetGlobalPosition().z));
    //    }
    //}
}


void HumanMovement::SetTilemap(Tilemap::HexagonalTilemap* tilemap)
{
    _tilemap = tilemap;
}

void HumanMovement::MoveTo(glm::vec3 destination)
{
    _targetDestination = destination;
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

    return node;
}

bool HumanMovement::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    _speed = node["speed"].as<float>();
    _forwardDetectionDistance = node["forwardDetectionDistance"].as<float>();

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
        ImGui::Text("Target Destination: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text("%f\t%f\t%f", _targetDestination.x, _targetDestination.y, _targetDestination.z);
        ImGui::PopFont();
    }
}
#endif