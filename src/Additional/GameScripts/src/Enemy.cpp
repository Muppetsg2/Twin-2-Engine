#include <Enemy.h>
#include <string>

YAML::Node Enemy::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "Enemy";
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool Enemy::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

#if _DEBUG
void Enemy::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Enemy##Component").append(id);
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