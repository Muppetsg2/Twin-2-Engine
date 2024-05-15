#include <core/DirectionalLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/CameraComponent.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

void DirectionalLightComponent::Initialize()
{
	OnChangePosition = [this](Transform* transform) {
		//light->position = transform->GetGlobalPosition();
		//light->direction = glm::vec3((transform->GetTransformMatrix() * glm::vec4(localDirection, 1.0f)));
		LightingController::Instance()->UpdateDLTransform(light);
	};

	OnViewerChange = [this]() {
		CameraComponent* camera = CameraComponent::GetMainCamera();
		CameraData data{
			.projection = camera->GetProjectionMatrix(),
			.view = camera->GetViewMatrix(),
			.pos = camera->GetTransform()->GetGlobalPosition(),
			.front = camera->GetFrontDir(),
			.farPlane = camera->GetFarPlane(),
			.isPerspective = camera->GetCameraType() == CameraType::PERSPECTIVE,
		};
		GetTransform()->SetGlobalPosition(LightingController::RecalculateDirLightSpaceMatrix(light, data));
	};

	//light->position = GetTransform()->GetGlobalPosition();

	glGenFramebuffers(1, &light->shadowMapFBO);
	
	glGenTextures(1, &light->shadowMap);
	glBindTexture(GL_TEXTURE_2D, light->shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		LightingController::SHADOW_WIDTH, LightingController::SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

void DirectionalLightComponent::Update()
{
	if (dirtyFlag) {
		LightingController::Instance()->UpdateDL(light);
		dirtyFlag = false;
	}
}

void DirectionalLightComponent::OnEnable()
{
	LightingController::Instance()->dirLights.insert(light);
	OnChangePositionId = GetTransform()->OnEventPositionChanged += OnChangePosition;
	OnViewerChangeId = LightingController::Instance()->ViewerTransformChanged += OnViewerChange;
	LightingController::Instance()->UpdateDirLights();
}

void DirectionalLightComponent::OnDisable()
{
	LightingController::Instance()->dirLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingController::Instance()->ViewerTransformChanged -= OnViewerChangeId;
	LightingController::Instance()->UpdateDirLights();
}

void DirectionalLightComponent::OnDestroy()
{
	LightingController::Instance()->dirLights.erase(light);
	GetTransform()->OnEventPositionChanged -= OnChangePositionId;
	LightingController::Instance()->ViewerTransformChanged -= OnViewerChangeId;
	LightingController::Instance()->UpdateDirLights();

	glDeleteFramebuffers(1, &light->shadowMapFBO);
	glDeleteTextures(GL_TEXTURE_2D, &light->shadowMap);

	delete light;
}

void DirectionalLightComponent::SetDirection(glm::vec3 dir)
{
	light->direction = dir;
	dirtyFlag = true;
}

void DirectionalLightComponent::SetColor(glm::vec3 color)
{
	light->color = color;
	dirtyFlag = true;
}

void DirectionalLightComponent::SetPower(float power)
{
	light->power = power;
	dirtyFlag = true;
}

YAML::Node DirectionalLightComponent::Serialize() const
{
	YAML::Node node = LightComponent::Serialize();
	node["type"] = "DirectionalLight";
	node["direction"] = light->direction;
	node["color"] = light->color;
	node["power"] = light->power;

	return node;
}

bool DirectionalLightComponent::Deserialize(const YAML::Node& node)
{
	if (!node["direction"] || !node["color"] || !node["power"] ||
		!LightComponent::Deserialize(node)) return false;

	light->direction = node["direction"].as<glm::vec3>();
	light->color = node["color"].as<glm::vec3>();
	light->power = node["power"].as<float>();

	return true;
}
