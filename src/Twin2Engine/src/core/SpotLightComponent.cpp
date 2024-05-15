#include <core/SpotLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

void SpotLightComponent::Initialize()
{
	OnChangeTransform = [this](Transform* transform) {
		light->position = GetTransform()->GetGlobalPosition();
		light->direction = glm::vec3((transform->GetTransformMatrix() * glm::vec4(localDirection, 1.0f)));
		LightingController::Instance()->UpdateSLTransform(light);
	};

	light->position = GetTransform()->GetGlobalPosition();
}

void SpotLightComponent::Update()
{
	if (dirtyFlag) {
		LightingController::Instance()->UpdateSL(light);
		dirtyFlag = false;
	}
}

void SpotLightComponent::OnEnable()
{
	LightingController::Instance()->spotLights.insert(light);
	OnChangeTransformId = GetTransform()->OnEventTransformChanged += OnChangeTransform;
	LightingController::Instance()->UpdateSpotLights();
}

void SpotLightComponent::OnDisable()
{
	LightingController::Instance()->spotLights.erase(light);
	GetTransform()->OnEventTransformChanged -= OnChangeTransformId;
	LightingController::Instance()->UpdateSpotLights();
}

void SpotLightComponent::OnDestroy()
{
	LightingController::Instance()->spotLights.erase(light);
	GetTransform()->OnEventTransformChanged -= OnChangeTransformId;
	LightingController::Instance()->UpdateSpotLights();

	delete light;
}

void SpotLightComponent::SetColor(glm::vec3 color)
{
	light->color = color;
	dirtyFlag = true;
}

void SpotLightComponent::SetDirection(glm::vec3 dir)
{
	light->direction = glm::vec3((GetTransform()->GetTransformMatrix() * glm::vec4(dir, 1.0f)));
	localDirection = dir;
	dirtyFlag = true;
}

void SpotLightComponent::SetPower(float power)
{
	light->power = power;
	dirtyFlag = true;
}

void SpotLightComponent::SetInnerCutOff(float radAngle)
{
	light->innerCutOff = radAngle;
	dirtyFlag = true;
}

void SpotLightComponent::SetOuterCutOff(float radAngle)
{
	light->outerCutOff = radAngle;
	dirtyFlag = true;
}

void SpotLightComponent::SetAttenuation(float constant, float linear, float quadratic)
{
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
	dirtyFlag = true;
}

YAML::Node SpotLightComponent::Serialize() const
{
	YAML::Node node = LightComponent::Serialize();
	node["type"] = "SpotLight";
	node["direction"] = light->direction;
	node["color"] = light->color;
	node["power"] = light->power;
	node["innerCutOff"] = light->innerCutOff;
	node["outerCutOff"] = light->outerCutOff;
	node["constant"] = light->constant;
	node["linear"] = light->linear;
	node["quadratic"] = light->quadratic;

	return node;
}

bool SpotLightComponent::Deserialize(const YAML::Node& node)
{
	if (!node["direction"] || !node["color"] || !node["power"] || !node["innerCutOff"] ||
		!node["outerCutOff"] || !node["constant"] || !node["linear"] || !node["quadratic"] ||
		!LightComponent::Deserialize(node)) return false;

	light->direction = node["direction"].as<glm::vec3>();
	light->color = node["color"].as<glm::vec3>();
	light->power = node["power"].as<float>();
	light->innerCutOff = node["innerCutOff"].as<float>();
	light->outerCutOff = node["outerCutOff"].as<float>();
	light->constant = node["constant"].as<float>();
	light->linear = node["linear"].as<float>();
	light->quadratic = node["quadratic"].as<float>();

	return true;
}

void SpotLightComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Spot Light##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		glm::vec3 v = light->direction;
		ImGui::DragFloat3(string("Direction##").append(id).c_str(), glm::value_ptr(v), .1f, -1.f, 1.f);
		if (v != light->direction) {
			SetDirection(v);
		}

		v = light->color;
		ImGui::ColorEdit3(string("Color##").append(id).c_str(), glm::value_ptr(v));
		if (v != light->color) {
			SetColor(v);
		}

		float p = light->power;
		ImGui::DragFloat(string("Power##").append(id).c_str(), &p);
		if (p != light->power) {
			SetPower(p);
		}

		p = light->innerCutOff;
		ImGui::DragFloat(string("Inner Cut Off##").append(id).c_str(), &p, 1.f, 0.f, 360.f);
		if (p != light->innerCutOff) {
			SetInnerCutOff(glm::radians(p));
		}

		p = light->outerCutOff;
		ImGui::DragFloat(string("Outer Cut Off##").append(id).c_str(), &p, 1.f, 0.f, 360.f);
		if (p != light->outerCutOff) {
			SetOuterCutOff(glm::radians(p));
		}

		float c = light->constant;
		float l = light->linear;
		p = light->quadratic;
		ImGui::DragFloat(string("Constant##").append(id).c_str(), &c);
		ImGui::SameLine();
		ImGui::DragFloat(string("Linear##").append(id).c_str(), &l);
		ImGui::SameLine();
		ImGui::DragFloat(string("Quadratic##").append(id).c_str(), &p);

		if (c != light->constant || l != light->linear || p != light->quadratic) {
			SetAttenuation(c, l, p);
		}
	}
}
