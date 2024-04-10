#include <core/DirectionalLightComponent.h>
#include <core/GameObject.h>
#include <core/Transform.h>


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


	SPDLOG_INFO("DLC Initialization!");

	light = new LightingSystem::DirectionalLight;
	light->position = GetTransform()->GetGlobalPosition();
	//LightingSystem::LightingController::Instance()->RecalculateDirLightSpaceMatrix(light);

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
	//GLfloat borderColor[] = { 0.0, 0.0, 0.0, 0.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	/*/int width, height, nrComponents;
	unsigned char* data = stbi_load("res/textures/XShadowMap2.png", &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, light->shadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << "res/textures/XShadowMap.png" << std::endl;
		stbi_image_free(data);
	}/**/


	glBindFramebuffer(GL_FRAMEBUFFER, light->shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void Twin2Engine::Core::DirectionalLightComponent::SetDirection(glm::vec3& dir)
{
	light->direction = dir;
	//light->direction = glm::vec3((GetTransform()->GetTransformMatrix() * glm::vec4(dir, 1.0f)));
	//localDirection = dir;
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
