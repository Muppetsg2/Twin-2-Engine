#include <core/SpotLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/YamlConverters.h>

void Twin2Engine::Core::SpotLightComponent::Initialize()
{
	OnChangeTransform = [this](Transform* transform) {
		light->position = GetTransform()->GetGlobalPosition();
		light->direction = glm::vec3((transform->GetTransformMatrix() * glm::vec4(localDirection, 1.0f)));
		LightingSystem::LightingController::Instance()->UpdateSLTransform(light);
	};


	//light = new LightingSystem::SpotLight;
	light->position = GetTransform()->GetGlobalPosition();
	//LightingSystem::LightingController::Instance()->spotLights.insert(light);
	//LightingSystem::LightingController::Instance()->UpdateSpotLights();
}

void Twin2Engine::Core::SpotLightComponent::Update()
{
	if (dirtyFlag) {
		LightingSystem::LightingController::Instance()->UpdateSL(light);
		dirtyFlag = false;
	}
}

void Twin2Engine::Core::SpotLightComponent::OnEnable()
{
	LightingSystem::LightingController::Instance()->spotLights.insert(light);
	OnChangeTransformId = GetTransform()->OnEventTransformChanged += OnChangeTransform;
	LightingSystem::LightingController::Instance()->UpdateSpotLights();
}

void Twin2Engine::Core::SpotLightComponent::OnDisable()
{
	LightingSystem::LightingController::Instance()->spotLights.erase(light);
	GetTransform()->OnEventTransformChanged -= OnChangeTransformId;
	LightingSystem::LightingController::Instance()->UpdateSpotLights();
}

void Twin2Engine::Core::SpotLightComponent::OnDestroy()
{
	LightingSystem::LightingController::Instance()->spotLights.erase(light);
	GetTransform()->OnEventTransformChanged -= OnChangeTransformId;
	LightingSystem::LightingController::Instance()->UpdateSpotLights();

	delete light;
}

void Twin2Engine::Core::SpotLightComponent::SetColor(glm::vec3 color)
{
	light->color = color;
	dirtyFlag = true;
}

void Twin2Engine::Core::SpotLightComponent::SetDirection(glm::vec3 dir)
{
	light->direction = glm::vec3((GetTransform()->GetTransformMatrix() * glm::vec4(dir, 1.0f)));
	localDirection = dir;
	dirtyFlag = true;
}

void Twin2Engine::Core::SpotLightComponent::SetPower(float power)
{
	light->power = power;
	dirtyFlag = true;
}

void Twin2Engine::Core::SpotLightComponent::SetOuterCutOff(float radAngle)
{
	light->outerCutOff = radAngle;
	dirtyFlag = true;
}

void Twin2Engine::Core::SpotLightComponent::SetAtenuation(float constant, float linear, float quadratic)
{
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
	dirtyFlag = true;
}

YAML::Node Twin2Engine::Core::SpotLightComponent::Serialize() const
{
	YAML::Node node = LightComponent::Serialize();
	node["direction"] = light->direction;
	node["color"] = light->color;
	node["power"] = light->power;
	node["outerCutOff"] = light->power;
	node["constant"] = light->constant;
	node["linear"] = light->linear;
	node["quadratic"] = light->quadratic;

	return node;
}
