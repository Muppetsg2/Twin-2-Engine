#include <Clouds/Cloud.h>
#include <Clouds/CloudController.h>
#include <tools/YamlConverters.h>
#include <core/Time.h>
#include <core/Random.h>


using namespace Twin2Engine::Core;

void Cloud::Initialize() {
	glm::vec3 positionOffset;
	positionOffset.x = Random::Range<float>(-0.4f, 0.4f) + 0.2;
	positionOffset.y = Random::Range<float>(-0.1f, 0.2f);
	positionOffset.z = Random::Range<float>(-0.4f, 0.4f) - 0.2;

	GetTransform()->Translate(positionOffset);

	startingScale.x = Random::Range<float>(0.25f, 0.4f);
	startingScale.y = Random::Range<float>(0.25f, 0.4f);
	startingScale.z = Random::Range<float>(0.25f, 0.4f);

	scaleVelocity.x = Random::Range<float>(-0.1f, 0.1f);
	scaleVelocity.y = Random::Range<float>(-0.1f, 0.1f);
	scaleVelocity.z = Random::Range<float>(-0.1f, 0.1f);

	glm::vec3 rotation(-90.0f, 0.0f, -0.0f);
	rotation.z = Random::Range<float>(-30.0f, 30.0f);
}

void Cloud::Update() {
	Transform* transform = GetTransform();

	//travelledDistance += velocity * Time::GetDeltaTime();
	//transform->SetGlobalPosition(startPosition + direction * travelledDistance);
	////transform->Translate(direction * velocity * Time::GetDeltaTime());
	//
	//if (travelledDistance > maxDistance) {
	//	transform->SetGlobalPosition(startPosition);
	//	travelledDistance = 0.0;
	//}
	
	startingScale += scaleVelocity * Time::GetDeltaTime();
	//transform->SetGlobalScale(startingScale);

	if (startingScale.x > 0.6f) {
		startingScale.x = 0.6f;
		scaleVelocity.x *= -1.0f;
	}
	else if (startingScale.x < 0.4f) {
		startingScale.x = 0.4f;
		scaleVelocity.x *= -1.0f;
	}
	if (startingScale.y > 0.6f) {
		startingScale.y = 0.6f;
		scaleVelocity.y *= -1.0f;
	}
	else if (startingScale.y < 0.4f) {
		startingScale.y = 0.4f;
		scaleVelocity.y *= -1.0f;
	}
	if (startingScale.z > 0.6f) {
		startingScale.z = 0.6f;
		scaleVelocity.z *= -1.0f;
	}
	else if (startingScale.z < 0.4f) {
		startingScale.z = 0.4f;
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
	node["direction"] = direction;
	node["velocity"] = velocity;
	node["startPosition"] = startPosition;
	node["startingScale"] = startingScale;
	node["scaleVelocity"] = scaleVelocity;
	node["maxDistance"] = maxDistance;
	node["travelledDistance"] = travelledDistance;
	return node;
}

bool Cloud::Deserialize(const YAML::Node& node) {
	if (!Component::Deserialize(node)) 
		return false;

	direction = node["direction"].as<glm::vec3>();
	velocity = node["velocity"].as<float>();
	startPosition = node["startPosition"].as<glm::vec3>();
	startingScale = node["startingScale"].as<glm::vec3>();
	scaleVelocity = node["scaleVelocity"].as<glm::vec3>();
	maxDistance = node["maxDistance"].as<float>();
	travelledDistance = node["travelledDistance"].as<float>();

	return true;
}

#if _DEBUG
void Cloud::DrawEditor() {
	string id = string(std::to_string(this->GetId()));
	string name = string("Cloud##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		ImGui::DragFloat3(string("Direction##").append(id).c_str(), glm::value_ptr(direction));
		ImGui::DragFloat3(string("Start Position##").append(id).c_str(), glm::value_ptr(startPosition));

		ImGui::DragFloat(string("Velocity##").append(id).c_str(), &velocity);
		ImGui::DragFloat(string("Max Distance##").append(id).c_str(), &maxDistance);

		ImGui::NewLine();
		ImGui::NewLine();

		//CloudController* cc = CloudController::Instance();

		// TODO: Kazda chmura pozyskuje te wartosci? Jesli tak to czemu kazda ma moc je zmieniac i czemu kazda je wyswietla

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