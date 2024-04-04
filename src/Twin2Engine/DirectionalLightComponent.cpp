#include <core/DirectionalLightComponent.h>
#include <core/GameObject.h>

void Twin2Engine::Core::DirectionalLightComponent::Initialize()
{
	OnChangeTransform = [this]() {
		light->position = getTransform()->GetGlobalPosition();
		light->direction = glm::vec3((getTransform()->GetTransformMatrix() * glm::vec4(localDirection, 1.0f)));
		LightingSystem::LightingController::Instance()->UpdateDLTransform(light);
	};

	light = new LightingSystem::DirectionalLight;
	light->position = getTransform()->GetGlobalPosition();
	//LightingSystem::LightingController::Instance()->dirLights.insert(light);
	//LightingSystem::LightingController::Instance()->UpdateDirLights();
}

void Twin2Engine::Core::DirectionalLightComponent::Update()
{
	if (dirtyFlag) {
		LightingSystem::LightingController::Instance()->UpdateDL(light);
		dirtyFlag = false;
	}
}

void Twin2Engine::Core::DirectionalLightComponent::OnEnable()
{
	LightingSystem::LightingController::Instance()->dirLights.insert(light);
	LightingSystem::LightingController::Instance()->UpdateDirLights();
}

void Twin2Engine::Core::DirectionalLightComponent::OnDisable()
{
	LightingSystem::LightingController::Instance()->dirLights.erase(light);
	LightingSystem::LightingController::Instance()->UpdateDirLights();
}

void Twin2Engine::Core::DirectionalLightComponent::OnDestroy()
{
	LightingSystem::LightingController::Instance()->dirLights.erase(light);
	LightingSystem::LightingController::Instance()->UpdateDirLights();
	delete light;
}

void Twin2Engine::Core::DirectionalLightComponent::SetDirection(glm::vec3& dir)
{
	light->direction = glm::vec3((getTransform()->GetTransformMatrix() * glm::vec4(dir, 1.0f)));
	localDirection = dir;
	dirtyFlag = true;
}

void Twin2Engine::Core::DirectionalLightComponent::SetColor(glm::vec3& color)
{
	light->color = color;
	dirtyFlag = true;
}

void Twin2Engine::Core::DirectionalLightComponent::SetPower(float& power)
{
	light->power = power;
	dirtyFlag = true;
}
