#include <Player.h>
#include <string>

using namespace Twin2Engine::Core;

YAML::Node Player::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "Player";
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool Player::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

void Player::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Player##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        Component::DrawInheritedFields();

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