#include <Clouds/Cloud.h>
#include <Clouds/CloudController.h>
#include <tools/YamlConverters.h>
#include <core/Time.h>
#include <core/Random.h>


using namespace Twin2Engine::Core;

void Cloud::Initialize() {
	startingScale.x = Random::Range<float>(0.08f, 1.2f);
	startingScale.y = Random::Range<float>(0.08f, 1.2f);
	startingScale.z = Random::Range<float>(0.08f, 1.2f);

	scaleVelocity.x = Random::Range<float>(-0.005f, 0.005f);
	scaleVelocity.y = Random::Range<float>(-0.005f, 0.005f);
	scaleVelocity.z = Random::Range<float>(-0.005f, 0.005f);
}

void Cloud::Update() {
	Transform* transform = GetTransform();
		
	startingScale += scaleVelocity * Time::GetDeltaTime();

	if (startingScale.x > 0.12f) {
		startingScale.x = 0.12f;
		scaleVelocity.x *= -1.0f;
	}
	else if (startingScale.x < 0.08f) {
		startingScale.x = 0.08f;
		scaleVelocity.x *= -1.0f;
	}
	if (startingScale.y > 0.12f) {
		startingScale.y = 0.12f;
		scaleVelocity.y *= -1.0f;
	}
	else if (startingScale.y < 0.08f) {
		startingScale.y = 0.08f;
		scaleVelocity.y *= -1.0f;
	}
	if (startingScale.z > 0.12f) {
		startingScale.z = 0.12f;
		scaleVelocity.z *= -1.0f;
	}
	else if (startingScale.z < 0.08f) {
		startingScale.z = 0.08f;
		scaleVelocity.z *= -1.0f;
	}

	transform->SetGlobalScale(startingScale);
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
	node["startingScale"] = startingScale;
	node["scaleVelocity"] = scaleVelocity;
	return node;
}

bool Cloud::Deserialize(const YAML::Node& node) {
	if (!Component::Deserialize(node)) 
		return false;

	startingScale = node["startingScale"].as<glm::vec3>();
	scaleVelocity = node["scaleVelocity"].as<glm::vec3>();

	return true;
}

#if _DEBUG
void Cloud::DrawEditor() {
	string id = string(std::to_string(this->GetId()));
	string name = string("Cloud##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		ImGui::DragFloat(string("ABSORPTION##").append(id).c_str(), &CloudController::ABSORPTION);
		ImGui::DragFloat(string("DENSITY_FAC##").append(id).c_str(), &CloudController::DENSITY_FAC);
		ImGui::DragFloat(string("NUMBER_OF_STEPS##").append(id).c_str(), &CloudController::NUMBER_OF_STEPS);
		ImGui::DragFloat(string("CLOUD_LIGHT_STEPS##").append(id).c_str(), &CloudController::CLOUD_LIGHT_STEPS);
		ImGui::DragFloat(string("CLOUD_LIGHT_MULTIPLIER##").append(id).c_str(), &CloudController::CLOUD_LIGHT_MULTIPLIER);
		ImGui::DragFloat(string("CLOUD_EXPOSURE##").append(id).c_str(), &CloudController::CLOUD_EXPOSURE);
		ImGui::DragFloat(string("POS_MULT##").append(id).c_str(), &CloudController::POS_MULT);
		ImGui::DragFloat(string("APLPHA_TRESHOLD##").append(id).c_str(), &CloudController::APLPHA_TRESHOLD);
		ImGui::DragFloat(string("DENSITY_TRESHOLD##").append(id).c_str(), &CloudController::DENSITY_TRESHOLD);
		ImGui::DragFloat3(string("NOISE_D_VEL_3D##").append(id).c_str(), glm::value_ptr(CloudController::NOISE_D_VEL_3D));

		CloudController::Instance()->UpdateCloudShaderUniforms();
	}
}
#endif