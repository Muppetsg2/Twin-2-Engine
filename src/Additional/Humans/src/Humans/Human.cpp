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
    //targetCity = CitiesManager.Instance.GetClosestCity(transform.position);
}

// Update is called once per frame
void Human::Update()
{
    if (glm::distance(GetTransform()->GetGlobalPosition(), _movement->GetTargetDestination()) <= achievingDestinationAccuracity)
    {
        vector<GameObject*> possibleTargetCities = CitiesManager::GetConnectedCities(targetCity);
        if (possibleTargetCities.size() > 0)
        {
            targetCity = possibleTargetCities[Random::Range(0ull, possibleTargetCities.size() - 1)];
            _movement->MoveTo(targetCity->GetTransform()->GetGlobalPosition());
        }
        else
        {
            //Debug.LogWarning("Lack of connected cities" + gameObject.GetInstanceID());
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

void Human::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Human##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {
        ImGui::InputFloat("AchievingDestinationAccuracity: ", &achievingDestinationAccuracity);
    }
}