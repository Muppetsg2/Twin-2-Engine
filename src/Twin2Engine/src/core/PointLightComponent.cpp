#include <core/PointLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;

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

void PointLightComponent::SetAtenuation(float constant, float linear, float quadratic)
{
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
	dirtyFlag = true;
}

YAML::Node PointLightComponent::Serialize() const
{
	YAML::Node node = LightComponent::Serialize();
	node["color"] = light->color;
	node["power"] = light->power;
	node["constant"] = light->constant;
	node["linear"] = light->linear;
	node["quadratic"] = light->quadratic;

	return node;
}
