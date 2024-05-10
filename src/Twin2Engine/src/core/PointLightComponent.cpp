#include <core/PointLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/YamlConverters.h>

void Twin2Engine::Core::PointLightComponent::Initialize()
{
	OnChangePosition = [this](Transform* transform) {
		light->position = transform->GetGlobalPosition();
		LightingSystem::LightingController::Instance()->UpdatePLPosition(light);
	};

	//light = new LightingSystem::PointLight;
	light->position = GetTransform()->GetGlobalPosition();
	//LightingSystem::LightingController::Instance()->pointLights.insert(light);
	//LightingSystem::LightingController::Instance()->UpdatePointLights();
}

void Twin2Engine::Core::PointLightComponent::Update()
{
	if (dirtyFlag) {
		LightingSystem::LightingController::Instance()->UpdatePL(light);
		dirtyFlag = false;
	}
}

void Twin2Engine::Core::PointLightComponent::OnEnable()
{
	LightingSystem::LightingController::Instance()->pointLights.insert(light);
	OnChangePositionId = GetTransform()->OnEventPositionChanged += OnChangePosition;
	LightingSystem::LightingController::Instance()->UpdatePointLights();
}

void Twin2Engine::Core::PointLightComponent::OnDisable()
{
	LightingSystem::LightingController::Instance()->pointLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingSystem::LightingController::Instance()->UpdatePointLights();
}

void Twin2Engine::Core::PointLightComponent::OnDestroy()
{
	LightingSystem::LightingController::Instance()->pointLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingSystem::LightingController::Instance()->UpdatePointLights();

	delete light;
}

void Twin2Engine::Core::PointLightComponent::SetColor(glm::vec3 color)
{
	light->color = color;
	dirtyFlag = true;
}

void Twin2Engine::Core::PointLightComponent::SetPower(float power)
{
	light->power = power;
	dirtyFlag = true;
}

void Twin2Engine::Core::PointLightComponent::SetAttenuation(float constant, float linear, float quadratic)
{
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
	dirtyFlag = true;
}

YAML::Node Twin2Engine::Core::PointLightComponent::Serialize() const
{
	YAML::Node node = LightComponent::Serialize();
	node["color"] = light->color;
	node["power"] = light->power;
	node["constant"] = light->constant;
	node["linear"] = light->linear;
	node["quadratic"] = light->quadratic;

	return node;
}

void Twin2Engine::Core::PointLightComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Point Light##").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
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