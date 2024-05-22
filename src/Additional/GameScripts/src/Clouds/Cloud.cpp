#include <Clouds/Cloud.h>
#include <Clouds/CloudController.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;

void Cloud::Initialize() {

}

void Cloud::Update() {

}

void Cloud::OnEnable() {
	CloudController::Instance()->RegisterCloud(this);
}

void Cloud::OnDisable() {
	CloudController::Instance()->UnregisterCloud(this);
}

void Cloud::OnDestroy() {
	CloudController::Instance()->UnregisterCloud(this);
}


YAML::Node Cloud::Serialize() const {
	YAML::Node node = Component::Serialize();
	node["type"] = "Cloud";
	node["direction"] = direction;
	node["velocity"] = velocity;
	node["startPosition"] = startPosition;
	node["maxDistance"] = maxDistance;
	return node;
}

bool Cloud::Deserialize(const YAML::Node& node) {
	if (!Component::Deserialize(node)) 
		return false;

	direction = node["direction"].as<glm::vec3>();
	velocity = node["velocity"].as<float>();
	startPosition = node["startPosition"].as<glm::vec3>();
	maxDistance = node["maxDistance"].as<float>();

	return true;
}

void Cloud::DrawEditor() {
	string id = string(std::to_string(this->GetId()));
	string name = string("Cloud##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		Component::DrawInheritedFields();

		ImGui::DragFloat3(string("direction##").append(id).c_str(), glm::value_ptr(direction));
		ImGui::DragFloat3(string("startPosition##").append(id).c_str(), glm::value_ptr(startPosition));

		ImGui::DragFloat(string("velocity##").append(id).c_str(), &velocity);
		ImGui::DragFloat(string("maxDistance##").append(id).c_str(), &maxDistance);
	}
}