#include <core/DirectionalLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/YamlConverters.h>


void Twin2Engine::Core::DirectionalLightComponent::Initialize()
{
	OnChangePosition = [this](Transform* transform) {
		light->position = transform->GetGlobalPosition();
		//light->direction = glm::vec3((transform->GetTransformMatrix() * glm::vec4(localDirection, 1.0f)));
		LightingSystem::LightingController::Instance()->UpdateDLTransform(light);
	};

	OnViewerChange = [this]() {
		GetTransform()->SetGlobalPosition(LightingSystem::LightingController::RecalculateDirLightSpaceMatrix(light));
	};


	//SPDLOG_INFO("DLC Initialization!");

	//light = new LightingSystem::DirectionalLight;
	light->position = GetTransform()->GetGlobalPosition();

	glGenFramebuffers(1, &light->shadowMapFBO);
	
	glGenTextures(1, &light->shadowMap);
	glBindTexture(GL_TEXTURE_2D, light->shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		LightingSystem::LightingController::SHADOW_WIDTH, LightingSystem::LightingController::SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, light->shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	OnChangePositionId = GetTransform()->OnEventPositionChanged += OnChangePosition;
	OnViewerChangeId = LightingSystem::LightingController::Instance()->ViewerTransformChanged += OnViewerChange;
	LightingSystem::LightingController::Instance()->UpdateDirLights();
}

void Twin2Engine::Core::DirectionalLightComponent::OnDisable()
{
	LightingSystem::LightingController::Instance()->dirLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingSystem::LightingController::Instance()->ViewerTransformChanged -= OnViewerChangeId;
	LightingSystem::LightingController::Instance()->UpdateDirLights();
}

void Twin2Engine::Core::DirectionalLightComponent::OnDestroy()
{
	LightingSystem::LightingController::Instance()->dirLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingSystem::LightingController::Instance()->ViewerTransformChanged -= OnViewerChangeId;
	LightingSystem::LightingController::Instance()->UpdateDirLights();

	glDeleteFramebuffers(1, &light->shadowMapFBO);
	glDeleteTextures(GL_TEXTURE_2D, &light->shadowMap);

	delete light;
}

void Twin2Engine::Core::DirectionalLightComponent::SetDirection(glm::vec3 dir)
{
	light->direction = dir;
	dirtyFlag = true;
}

void Twin2Engine::Core::DirectionalLightComponent::SetColor(glm::vec3 color)
{
	light->color = color;
	dirtyFlag = true;
}

void Twin2Engine::Core::DirectionalLightComponent::SetPower(float power)
{
	light->power = power;
	dirtyFlag = true;
}

YAML::Node Twin2Engine::Core::DirectionalLightComponent::Serialize() const
{
	YAML::Node node = LightComponent::Serialize();
	node["direction"] = light->direction;
	node["color"] = light->color;
	node["power"] = light->power;

	return node;
}

void Twin2Engine::Core::DirectionalLightComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Directional Light##").append(id);
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
	}
}