#include <Humans/Human.h>

using namespace Humans;
using namespace Twin2Engine::Core;
using namespace Generation;

using namespace glm;
using namespace std;

// Start is called before the first frame update
void Human::Initialize()
{
    _movement = GetGameObject()->GetComponent<HumanMovement>();
}

void Human::StartWorking()
{
    _work = true;
    _targetCity = CitiesManager::GetClosestCity(GetTransform()->GetGlobalPosition());
    vec3 destination = _targetCity->GetTransform()->GetGlobalPosition();
    //destination.y = 0.0f;
    _movement->MoveTo(destination);
}

void Human::StopWorking()
{
    _work = false;
}

// Update is called once per frame
void Human::Update()
{
    if (_work)
    {
        vec3 globalPosition = GetTransform()->GetGlobalPosition();
        globalPosition.y = 0.0f;
        if (glm::distance(globalPosition, _movement->GetTargetDestination()) <= achievingDestinationAccuracity)
        {
            vector<GameObject*> possibleTargetCities = CitiesManager::GetConnectedCities(_targetCity);
            if (possibleTargetCities.size() > 0)
            {
                _targetCity = possibleTargetCities[Random::Range(0ull, possibleTargetCities.size() - 1ull)];
                vec3 destination = _targetCity->GetTransform()->GetGlobalPosition();
                destination.y = 0.0f;
                _movement->MoveTo(destination);
            }
            else
            {
                SPDLOG_INFO("Lack of connected cities");
            }
        }
    }
}



YAML::Node Human::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "Human";
    node["achievingDestinationAccuracity"] = achievingDestinationAccuracity;

    return node;
}

bool Human::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    achievingDestinationAccuracity = node["achievingDestinationAccuracity"].as<float>();

    return true;
}

#if _DEBUG
void Human::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Human##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {
        ImGui::InputFloat("AchievingDestinationAccuracity: ", &achievingDestinationAccuracity);
    }
}
#endif