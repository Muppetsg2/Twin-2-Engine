#include <graphic/LightingController.h>
#include <graphic/Window.h>

#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/ShaderManager.h>

#define max max

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Tools;

LightingController* LightingController::instance = nullptr;
const int LightingController::SHADOW_WIDTH = 2048;
const int LightingController::SHADOW_HEIGHT = 2048;
const int LightingController::MAPS_BEGINNING = 27;
float LightingController::DLShadowCastingRange = 20.0f;
glm::vec3 LightingController::viewerPosition(0.0f);
bool LightingController::lastViewerPositionSet = false;
glm::vec3 LightingController::lastViewerPosition(0.0f);

LightingController::LightingController() {
	glGenBuffers(1, &LightsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Lights), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, LightsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	glGenBuffers(1, &LightingDataBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightingData), NULL, GL_STATIC_DRAW); 
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, LightingDataBuffer);
	LightingData lightingData;
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightingData), &lightingData);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

LightingController::~LightingController() {
	glDeleteBuffers(1, &LightsBuffer);
	glDeleteBuffers(1, &LightingDataBuffer);
	pointLights.clear();
	spotLights.clear();
	dirLights.clear();
}

LightingController* LightingController::Instance() {
	if (instance == nullptr) {
		instance = new LightingController();

		//instance->SetAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));
		instance->SetAmbientLight(glm::vec3(0.3f, 0.3f, 0.3f));
		//instance->SetHighlightParam(2.0f);
	}

	return instance;
}

bool LightingController::IsInstantiated() {
	return instance != nullptr;
}

void LightingController::UnloadAll() {
	delete instance;
}

void LightingController::UpdateLightsBuffer() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	Lights lights;
	auto itr1 = pointLights.begin();
	while (itr1 != pointLights.end() && lights.numberOfPointLights < MAX_POINT_LIGHTS) {
		lights.pointLights[lights.numberOfPointLights] = **itr1;
		++itr1;
		++lights.numberOfPointLights;
	}
	auto itr2 = spotLights.begin();
	while (itr2 != spotLights.end() && lights.numberOfSpotLights < MAX_SPOT_LIGHTS) {
		lights.spotLights[lights.numberOfSpotLights] = **itr2;
		++itr2;
		++lights.numberOfSpotLights;
	}
	auto itr3 = dirLights.begin();
	while (itr3 != dirLights.end() && lights.numberOfDirLights < MAX_DIRECTIONAL_LIGHTS) {
		lights.directionalLights[lights.numberOfDirLights] = **itr3;
		RecalculateDirLightSpaceMatrix(*itr3);
		++itr3;
		++lights.numberOfDirLights;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Lights), &lights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdatePointLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int plNumber = 0;
	PointLight pLights[8];
	auto itr = pointLights.begin();
	while (itr != pointLights.end() && plNumber < MAX_POINT_LIGHTS) {
		pLights[plNumber] = **itr;
		++itr;
		++plNumber;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, numberOfPointLights), sizeof(unsigned int), &plNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, pointLights), plNumber * sizeof(PointLight), &pLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateSpotLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int slNumber = 0;
	SpotLight sLights[8];
	auto itr = spotLights.begin();
	while (itr != spotLights.end() && slNumber < MAX_SPOT_LIGHTS) {
		sLights[slNumber] = **itr;
		++itr;
		++slNumber;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, numberOfSpotLights), sizeof(unsigned int), &slNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, spotLights), slNumber * sizeof(SpotLight), &sLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateDirLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int dlNumber = 0;
	DirectionalLight dLights[4];
	auto itr = dirLights.begin();
	while (itr != dirLights.end() && dlNumber < MAX_DIRECTIONAL_LIGHTS) {
		dLights[dlNumber] = **itr;
		++itr;
		++dlNumber;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, numberOfDirLights), sizeof(unsigned int), &dlNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, directionalLights), dlNumber * sizeof(DirectionalLight), &dLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdatePLPosition(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	if (pos < MAX_POINT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, pointLights) + pos * sizeof(PointLight), sizeof(PointLight), &((*itr)->position));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateSLTransform(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	if (pos < MAX_SPOT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, spotLights) + pos * sizeof(SpotLight), sizeof(SpotLight), *itr);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateDLTransform(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	if (pos < MAX_DIRECTIONAL_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, directionalLights) + pos * sizeof(DirectionalLight), sizeof(DirectionalLight), *itr);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}


void LightingController::UpdatePL(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	if (pos < MAX_POINT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, pointLights) + pos * sizeof(PointLight), sizeof(PointLight), *itr);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateSL(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	if (pos < MAX_SPOT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, spotLights) + pos * sizeof(SpotLight), sizeof(SpotLight), *itr);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateDL(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	if (pos < MAX_DIRECTIONAL_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetof(Lights, directionalLights) + pos * sizeof(DirectionalLight), sizeof(DirectionalLight), *itr);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}


void LightingController::BindLightBuffors(Twin2Engine::Graphic::Shader* shader) {
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, LightsBuffer);
	//glBindBufferBase(GL_UNIFORM_BUFFER, 3, LightingDataBuffer);

	shader->Use();
	//shader->SetInt("DirLightShadowMaps[0]", MAPS_BEGINNING);
	//shader->SetInt("DirLightShadowMaps[1]", MAPS_BEGINNING + 1);
	//shader->SetInt("DirLightShadowMaps[2]", MAPS_BEGINNING + 2);
	//shader->SetInt("DirLightShadowMaps[3]", MAPS_BEGINNING + 3);
	std::string str = "DirLightShadowMaps[";
	for (int i = 0; i < 4; ++i) {
		shader->SetInt(str.append(std::to_string(i)).append("]").c_str(), MAPS_BEGINNING + i);
	}
}
/*/
void LightingController::UpdateShadowMapsTab(Twin2Engine::GraphicEngine::Shader* shader) {
	int i = 0;
	shader->Use();
	SPDLOG_INFO("Aktualizacja shadow mapy");

	for (auto dirLight : dirLights) { //DirLightShadowMaps
		glActiveTexture(GL_TEXTURE0 + MAPS_BEGINNING + i);
		glBindTexture(GL_TEXTURE_2D, dirLight->shadowMap);
		++i;
	}
}/**/

glm::vec3 LightingController::RecalculateDirLightSpaceMatrix(DirectionalLight* light) { //const glm::mat4& viewProjectionInverse
	/**/
	//Twin2Engine::Core::CameraComponent* mainCam = Twin2Engine::Core::CameraComponent::GetMainCamera();
	//
	//float zLength = 100.0f;
	//glm::vec3 lightNewPos = viewerPosition + mainCam->GetFrontDir() * DLShadowCastingRange - light->direction * (zLength * 0.5f);
	//glm::mat4 viewMatrix = glm::lookAt(lightNewPos, lightNewPos + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));
	//
	//float orthoHeight = 20.0f;
	//float orthoWidth = 20.0f;
	//
	////light->lightSpaceMatrix = glm::ortho(-(maxX + lightMargin), maxX + lightMargin, -(maxY + lightMargin), maxY + lightMargin, -zLength, zLength) * viewMatrix;/**/
	//light->lightSpaceMatrix = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -zLength, zLength) * viewMatrix;/**/
	////light->lightSpaceMatrix = glm::ortho(-maxX * MarginScaleX + XMovement, maxX * MarginScaleX + XMovement, -(maxY * MarginScaleX), maxY * MarginScaleX, -zLength, zLength) * viewMatrix;/**/
	//
	//lastViewerPosition = viewerPosition;
	//lastViewerPositionSet = true;
	//
	//return std::move(lightNewPos);
	Twin2Engine::Core::CameraComponent* mainCam = Twin2Engine::Core::CameraComponent::GetMainCamera();

	float zLength = 100.0f;
	glm::vec3 offset(-18.0f, 0.0f, 18.0f);
	glm::vec3 lightNewPos = viewerPosition + mainCam->GetFrontDir() * DLShadowCastingRange - light->direction * (zLength * 0.5f) + offset;
	glm::mat4 viewMatrix = glm::lookAt(lightNewPos, lightNewPos + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));

	float orthoHeight = 13.0f;
	float orthoWidth = 13.0f;

	//light->lightSpaceMatrix = glm::ortho(-(maxX + lightMargin), maxX + lightMargin, -(maxY + lightMargin), maxY + lightMargin, -zLength, zLength) * viewMatrix;/**/
	light->lightSpaceMatrix = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -zLength, zLength) * viewMatrix;/**/
	//light->lightSpaceMatrix = glm::ortho(-maxX * MarginScaleX + XMovement, maxX * MarginScaleX + XMovement, -(maxY * MarginScaleX), maxY * MarginScaleX, -zLength, zLength) * viewMatrix;/**/

	lastViewerPosition = viewerPosition;
	lastViewerPositionSet = true;

	ShaderManager::CloudLightDepthShader->Use();
	ShaderManager::CloudLightDepthShader->SetMat4("projectionViewMatrix", light->lightSpaceMatrix);

	return std::move(lightNewPos);
}

void LightingController::RenderShadowMaps() {
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	int i = 0;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	for (auto light : dirLights) {
		//Twin2Engine::Manager::MeshRenderingManager::RenderDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT, light->shadowMapFBO, light->lightSpaceMatrix);
		Twin2Engine::Manager::MeshRenderingManager::RenderDepthMapStatic(light->shadowMapFBO, light->shadowMap, light->shadowMapDynamic, light->lightSpaceMatrix);
		//Twin2Engine::Manager::MeshRenderingManager::RenderDepthMapDynamic(light->shadowMapFBO, light->lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0 + MAPS_BEGINNING + i);
		glBindTexture(GL_TEXTURE_2D, light->shadowMap);

		++i;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glm::ivec2 wSize = Twin2Engine::Graphic::Window::GetInstance()->GetContentSize();
	glViewport(0, 0, wSize.x, wSize.y);

	glCullFace(GL_BACK);
}

void LightingController::RenderDynamicShadowMaps() {
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	int i = 0;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	for (auto light : dirLights) {
		//Twin2Engine::Manager::MeshRenderingManager::RenderDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT, light->shadowMapFBO, light->lightSpaceMatrix);
		Twin2Engine::Manager::MeshRenderingManager::RenderDepthMapDynamic(light->shadowMapFBO, light->lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0 + MAPS_BEGINNING + 2 * i + 1);
		glBindTexture(GL_TEXTURE_2D, light->shadowMapDynamic);

		++i;
	}

	glm::ivec2 wSize = Twin2Engine::Graphic::Window::GetInstance()->GetContentSize();
	glViewport(0, 0, wSize.x, wSize.y);

	glCullFace(GL_BACK);
}

void LightingController::SetAmbientLight(glm::vec3 ambientLightColor) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &ambientLightColor);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingController::SetShininness(float shininness) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightingData, shininness), sizeof(unsigned int), &shininness);
	glBindBuffer(GL_UNIFORM_BUFFER, NULL);
}

void LightingController::SetViewerPosition(glm::vec3& viewerPosition) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightingData, viewerPosition), sizeof(glm::vec3), &viewerPosition);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	UpdateOnTransformChange();
}

void LightingController::SetShadingType(int type) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightingData, shadingType), sizeof(unsigned int), &type);
	glBindBuffer(GL_UNIFORM_BUFFER, NULL);
}