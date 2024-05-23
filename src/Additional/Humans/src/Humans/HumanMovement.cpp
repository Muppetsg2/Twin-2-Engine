#include <Humans/HumanMovement.h>

using namespace Humans;
using namespace Generation;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

using namespace glm;
using namespace std;

void HumanMovement::Initialize()
{
    _targetDestination = GetTransform()->GetGlobalPosition();

}

void HumanMovement::Update()
{
    glm::vec3 globalPosition = GetTransform()->GetGlobalPosition();
    glm::vec3 direction = normalize((_targetDestination - globalPosition));

    GetTransform()->Translate(direction * _speed * Time::GetDeltaTime());



    RaycastHit forawrdHit; // = Physics.RaycastAll(transform.position + new Vector3(0, 1, 0) + direction * _forwardDetectionDistance, Vector3.down, 5f);
    RaycastHit currentHit; // = Physics.RaycastAll(transform.position + new Vector3(0, 1, 0), Vector3.down, 5f);

    Ray forwardRay(vec3(0.0f, -1.0f, 0.0f), globalPosition + vec3(0, 0.1, 0) + direction * _forwardDetectionDistance);
    Ray currentRay(vec3(0.0f, -1.0f, 0.0f), globalPosition + vec3(0, 0.1, 0));

    CollisionManager::Instance()->Raycast(forwardRay, forawrdHit);
    CollisionManager::Instance()->Raycast(currentRay, currentHit);

    //Debug.Log("Hits: " + forawrdHit.Length);
    MapHexTile* forwardHexTile = nullptr;
    //SPDLOG_INFO("Found frwdRay: {}", forawrdHit.collider != nullptr);
    if (forawrdHit.collider)
    {
        forwardHexTile = forawrdHit.collider->GetGameObject()->GetComponent<MapHexTile>();
    }
    //for (int i = 0; i < forawrdHit.Length; i++)
    //{
    //    forwardHexTile = forawrdHit[i].collider.GetComponent<MapHexTile>();
    //    if (forwardHexTile != nullptr)
    //    {
    //        //transform.position = new Vector3(transform.position.x, forwardHexTile.transform.position.y, transform.position.z);
    //
    //        break;
    //    }
    //}
    //
    //SPDLOG_INFO("Found crrntRay: {}", currentHit.collider != nullptr);
    MapHexTile* currentHexTile = nullptr;
    if (currentHit.collider)
    {
        currentHexTile = currentHit.collider->GetGameObject()->GetComponent<MapHexTile>();
    }
    //for (int i = 0; i < currentHit.Length; i++)
    //{
    //    currentHexTile = currentHit[i].collider.GetComponent<MapHexTile>();
    //    if (currentHexTile != nullptr)
    //    {
    //        //transform.position = new Vector3(transform.position.x, currentHexTile.transform.position.y, transform.position.z);
    //
    //        break;
    //    }
    //}
    if (currentHexTile != nullptr && forwardHexTile != nullptr)
    {
        float currentPositionY = currentHexTile->GetTransform()->GetGlobalPosition().y + 0.06f;
        float forwardPositionY = forwardHexTile->GetTransform()->GetGlobalPosition().y + 0.06f;
        if (currentPositionY > forwardPositionY)
        {
            GetTransform()->SetGlobalPosition(vec3(GetTransform()->GetGlobalPosition().x, currentPositionY, GetTransform()->GetGlobalPosition().z));
        }
        else
        {
            GetTransform()->SetGlobalPosition(vec3(GetTransform()->GetGlobalPosition().x, forwardPositionY, GetTransform()->GetGlobalPosition().z));
        }
    }
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
        ImGui::InputFloat("Speed: ", &_forwardDetectionDistance);
        ImGui::Text("Target Destination: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text("%f\t%f\t%f", _targetDestination.x, _targetDestination.y, _targetDestination.z);
        ImGui::PopFont();
    }
}
#endif