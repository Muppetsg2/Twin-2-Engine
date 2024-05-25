#include <core/PointLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>
#include <graphic/LightingController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

void PointLightComponent::Initialize()
{
	OnChangePosition = [this](Transform* transform) {
		light->position = transform->GetGlobalPosition();
		LightingController::Instance()->UpdatePLPosition(light);
	};

	light->position = GetTransform()->GetGlobalPosition();
}

void PointLightComponent::Update()
{
	if (dirtyFlag) {
		LightingController::Instance()->UpdatePL(light);
		dirtyFlag = false;
	}
}

void PointLightComponent::OnEnable()
{
	LightingController::Instance()->pointLights.insert(light);
	OnChangePositionId = GetTransform()->OnEventPositionChanged += OnChangePosition;
	LightingController::Instance()->UpdatePointLights();
}

void PointLightComponent::OnDisable()
{
	LightingController::Instance()->pointLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingController::Instance()->UpdatePointLights();
}

void PointLightComponent::OnDestroy()
{
	LightingController::Instance()->pointLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingController::Instance()->UpdatePointLights();

	delete light;
}

void PointLightComponent::SetColor(glm::vec3 color)
{
	light->color = color;
	dirtyFlag = true;
}

void PointLightComponent::SetPower(float power)
{
	light->power = power;
	dirtyFlag = true;
}

void PointLightComponent::SetAttenuation(float constant, float linear, float quadratic)
{
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
	dirtyFlag = true;
}

YAML::Node PointLightComponent::Serialize() const
{
	YAML::Node node = LightComponent::Serialize();
	node["type"] = "PointLight";
	node["color"] = light->color;
	node["power"] = light->power;
	node["constant"] = light->constant;
	node["linear"] = light->linear;
	node["quadratic"] = light->quadratic;

	return node;
}

bool PointLightComponent::Deserialize(const YAML::Node& node)
{
	if (!node["color"] || !node["power"] || !node["constant"] || !node["linear"] || !node["quadratic"] ||
		!LightComponent::Deserialize(node)) return false;

	light->color = node["color"].as<glm::vec3>();
	light->power = node["power"].as<float>();
	light->constant = node["constant"].as<float>();
	light->linear = node["linear"].as<float>();
	light->quadratic = node["quadratic"].as<float>();

	return true;
}

#if _DEBUG
void PointLightComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Point Light##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;
		glm::vec3 v = light->color;
		ImGui::ColorEdit3(string("Color##").append(id).c_str(), glm::value_ptr(v));
		if (v != light->color) {
			SetColor(v);
		}

		float p = light->power;
		ImGui::DragFloat(string("Power##").append(id).c_str(), &p);
		if (p != light->power) {
			SetPower(p);
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
#endif