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
		CameraComponent* camera = CameraComponent::GetMainCamera();
		LightingSystem::CameraData data{
			.projection = camera->GetProjectionMatrix(),
			.view = camera->GetViewMatrix(),
			.pos = camera->GetTransform()->GetGlobalPosition(),
			.front = camera->GetFrontDir(),
			.farPlane = camera->GetFarPlane(),
			.isPerspective = camera->GetCameraType() == CameraType::PERSPECTIVE,
		};
		GetTransform()->SetGlobalPosition(LightingSystem::LightingController::RecalculateDirLightSpaceMatrix(light, data));
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
