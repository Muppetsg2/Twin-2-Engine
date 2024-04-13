#include <core/PointLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>

void Twin2Engine::Core::PointLightComponent::Initialize()
{
	OnChangePosition = [this](Transform* transform) {
		light->position = transform->GetGlobalPosition();
		LightingSystem::LightingController::Instance()->UpdatePLPosition(light);
	};

	light = new LightingSystem::PointLight;
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

void Twin2Engine::Core::PointLightComponent::SetAtenuation(float constant, float linear, float quadratic)
{
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
	dirtyFlag = true;
}
