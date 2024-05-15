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

void SpotLightComponent::SetOuterCutOff(float radAngle)
{
	light->outerCutOff = radAngle;
	dirtyFlag = true;
}

void SpotLightComponent::SetAtenuation(float constant, float linear, float quadratic)
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
