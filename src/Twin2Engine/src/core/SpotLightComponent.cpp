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

void Twin2Engine::Core::SpotLightComponent::SetInnerCutOff(float radAngle)
{
	light->innerCutOff = radAngle;
	dirtyFlag = true;
}

void Twin2Engine::Core::SpotLightComponent::SetOuterCutOff(float radAngle)
{
	light->outerCutOff = radAngle;
	dirtyFlag = true;
}

void Twin2Engine::Core::SpotLightComponent::SetAttenuation(float constant, float linear, float quadratic)
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
	node["innerCutOff"] = light->innerCutOff;
	node["outerCutOff"] = light->outerCutOff;
	node["constant"] = light->constant;
	node["linear"] = light->linear;
	node["quadratic"] = light->quadratic;

	return node;
}

void Twin2Engine::Core::SpotLightComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Spot Light##").append(id);
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