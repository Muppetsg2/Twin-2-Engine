#include "LightingController.h"

#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <core/CameraComponent.h>
#include <graphic/manager/MeshRenderingManager.h>
#include <core/GameObject.h>


#include <utility>
#include <string>

using namespace LightingSystem;

LightingController* LightingController::instance = nullptr;
const int LightingController::SHADOW_WIDTH = 1024;
const int LightingController::SHADOW_HEIGHT = 1024;
float LightingController::DLShadowCastingRange = 15.0f;

LightingController* LightingController::Instance() {
	if (instance == nullptr) {
		instance = new LightingController();
	}

	return instance;
}

bool LightingController::IsInstantiated() {
	return instance != nullptr;
}

LightingController::LightingController() {
	glGenBuffers(1, &LightsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Lights), &lights, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Lights), nullptr, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, LightsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	glGenBuffers(1, &LightingDataBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightingData), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, LightingDataBuffer);
	LightingData lightingData;
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 32, &lightingData);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingController::UpdateLightsBuffer() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	Lights lights;
	/*lights.numberOfPointLights = pointLights.size();
	lights.numberOfSpotLights = spotLights.size();
	lights.numberOfDirLights = dirLights.size();/**/
	auto itr1 = pointLights.begin();
	while (itr1 != pointLights.end()) {
		lights.pointLights[lights.numberOfPointLights] = **itr1;
		++itr1;
		++lights.numberOfPointLights;
	}
	auto itr2 = spotLights.begin();
	while (itr2 != spotLights.end()) {
		lights.spotLights[lights.numberOfSpotLights] = **itr2;
		++itr2;
		++lights.numberOfSpotLights;
	}
	auto itr3 = dirLights.begin();
	while (itr3 != dirLights.end()) {
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
	while (itr != pointLights.end()) {
		pLights[plNumber] = **itr;
		++itr;
		++plNumber;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &plNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, plNumber * 48, &pLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateSpotLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int slNumber = 0;
	SpotLight sLights[8];
	auto itr = spotLights.begin();
	while (itr != spotLights.end()) {
		sLights[slNumber] = **itr;
		++itr;
		++slNumber;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 4, 4, &slNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 400, slNumber * 64, &sLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateDirLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int dlNumber = 0;
	DirectionalLight dLights[4];
	auto itr = dirLights.begin();
	while (itr != dirLights.end()) {
		dLights[dlNumber] = **itr;
		//RecalculateDirLightSpaceMatrix(*itr);
		++itr;
		++dlNumber;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 8, 4, &dlNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 912, dlNumber * 112, &dLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//Twin2Engine::Manager::ShaderManager::UpdateDirShadowMapsTab();
}


void LightingController::UpdatePLPosition(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + pos * 48, 12, &((*itr)->position));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateSLTransform(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 396 + pos * sizeof(SpotLight), 12, &((*itr)->position));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 400 + pos * 64, 28, *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateDLTransform(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	//RecalculateDirLightSpaceMatrix(dirLight);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 912 + pos * sizeof(DirectionalLight), 12, &((*itr)->position)); //972
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 912 + pos * 112, 96, *itr); //972
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//SPDLOG_INFO("Aktualizacja DL transform");
}


void LightingController::UpdatePL(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + pos * 48, 48, *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateSL(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 400 + pos * 64, 64, *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateDL(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 912 + pos * 112, 112, *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	SPDLOG_INFO("Aktualizacja DL");
}


void LightingController::BindLightBuffors(Twin2Engine::GraphicEngine::Shader* shader) {
	GLuint block_index = 0;

	block_index = glGetProgramResourceIndex(shader->shaderProgramID, GL_SHADER_STORAGE_BLOCK, "Lights");
	glShaderStorageBlockBinding(shader->shaderProgramID, block_index, 3);

	block_index = glGetUniformBlockIndex(shader->shaderProgramID, "LightingData");
	glUniformBlockBinding(shader->shaderProgramID, block_index, 4);

	std::string str = "DirLightShadowMaps[";
	shader->Use();
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "0]").c_str()), 8);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "1]").c_str()), 9);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "2]").c_str()), 10);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "3]").c_str()), 11);
	//UpdateShadowMapsTab(shader);
}

void LightingController::UpdateShadowMapsTab(Twin2Engine::GraphicEngine::Shader* shader) {
	int i = 0;
	shader->Use();
	SPDLOG_INFO("Aktualizacja shadow mapy");

	for (auto dirLight : dirLights) { //DirLightShadowMaps
		glActiveTexture(GL_TEXTURE0 + 8 + i);
		glBindTexture(GL_TEXTURE_2D, dirLight->shadowMap);
		//glBindTexture(GL_TEXTURE0 + 8 + i, dirLight->shadowMap);
		//glUniform1i(glGetUniformLocation(shader->shaderProgramID, ("DirLightShadowMaps[" + std::to_string(i) + "]").c_str()), dirLight->shadowMap);
		//glUniform1i(glGetUniformLocation(shader->shaderProgramID, ("DirLightShadowMaps[" + std::to_string(i) + "]").c_str()), 8 + i);
		++i;
	}
}

glm::vec3 LightingController::RecalculateDirLightSpaceMatrix(DirectionalLight* light) { //const glm::mat4& viewProjectionInverse
	/**/
	Twin2Engine::Core::CameraComponent* mainCam = Twin2Engine::Core::CameraComponent::GetMainCamera();
	glm::mat4 projectionViewInverse = glm::inverse(mainCam->GetProjectionMatrix() * mainCam->GetViewMatrix());
	std::vector<glm::vec3> corners;

	glm::vec3 lightNewPos;
	glm::vec3 frustumCenter;

	if (mainCam->GetCameraType() == Twin2Engine::Core::CameraType::ORTHOGRAPHIC) {
		std::vector<glm::vec4> ndcNearCorners = {
			{ -1.0f,  1.0f, -1.0f, 1.0f },
			{  1.0f,  1.0f, -1.0f, 1.0f },
			{  1.0f, -1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f, -1.0f, 1.0f },
		};
		////do robiæ
	}
	else {
		glm::vec3 camPos = mainCam->GetTransform()->GetGlobalPosition();
		corners.push_back(camPos);
		frustumCenter = camPos + mainCam->GetFrontDir() * (DLShadowCastingRange * 0.5f);

		float ndcZPos = DLShadowCastingRange / mainCam->GetFarPlane();
		glm::vec3 gPos = projectionViewInverse * glm::vec4(-1.0f, 1.0f, ndcZPos * DLShadowCastingRange, 1.0f);
		corners.push_back(gPos);
		gPos = projectionViewInverse * glm::vec4(1.0f, 1.0f, ndcZPos * DLShadowCastingRange, 1.0f);
		corners.push_back(gPos);
		gPos = projectionViewInverse * glm::vec4(1.0f, -1.0f, ndcZPos * DLShadowCastingRange, 1.0f);
		corners.push_back(gPos);
		gPos = projectionViewInverse * glm::vec4(-1.0f, -1.0f, ndcZPos * DLShadowCastingRange, 1.0f);
		corners.push_back(gPos);
	}


	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = minX;
	float maxY = maxX;
	float minZ = minX;
	float maxZ = maxX;

	for (const auto& corner : corners) {
		minX = std::min(minX, corner.x);
		maxX = std::max(maxX, corner.x);
		minY = std::min(minY, corner.y);
		maxY = std::max(maxY, corner.y);
		minZ = std::min(minZ, corner.z);
		maxZ = std::max(maxZ, corner.z);
	}

	glm::vec3 origin = { 0.0f, 0.0f, 0.0f };
	if (light->direction.x < 0) {
		origin.x = maxX;
	}
	else {
		origin.x = minX;
	}
	if (light->direction.y < 0) {
		origin.y = maxY;
	}
	else {
		origin.y = minY;
	}
	if (light->direction.z < 0) {
		origin.z = maxZ;
	}
	else {
		origin.z = minZ;
	}

	glm::vec3 V = frustumCenter - origin;
	float VoDir = glm::dot(V, light->direction);
	//Zawiera now¹ pozycjê œwiat³a
	lightNewPos = frustumCenter - 20.0f * VoDir * light->direction;

	glm::mat4 viewMatrix = glm::lookAt(lightNewPos, lightNewPos + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));

	maxX = 0.0f;
	maxY = 0.0f;
	for (const auto& corner : corners) {
		glm::vec3 transformedCorner = glm::vec3(viewMatrix * glm::vec4(corner, 1.0f));
		maxX = std::max(maxX, glm::abs(corner.x));
		maxY = std::max(maxY, glm::abs(corner.y));
	}


	// Adjust minZ and maxZ for better precision in the depth buffer
	float lightMargin = 5.0f; // Adjust based on scene size

	float zLength = 40.0f * glm::abs(VoDir);
	light->lightSpaceMatrix = glm::ortho(-(maxX + lightMargin), maxX + lightMargin, -(maxY + lightMargin), maxY + lightMargin, -zLength, zLength) * viewMatrix;/**/

	/*/glm::mat4 viewProjectionInverse = glm::inverse(mainCam->GetProjectionMatrix() * mainCam->GetViewMatrix());
	
	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = minX;
	float maxY = maxX;
	float minZ = minX;
	float maxZ = maxX;

	std::vector<glm::vec3> corners;

	// Define NDC cube in homogeneous coordinates
	std::vector<glm::vec4> ndcCorners = {
		{ -1.0f,  1.0f, -1.0f, 1.0f },
		{  1.0f,  1.0f, -1.0f, 1.0f },
		{  1.0f, -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f, 1.0f },
		{ -1.0f,  1.0f,  1.0f, 1.0f },
		{  1.0f,  1.0f,  1.0f, 1.0f },
		{  1.0f, -1.0f,  1.0f, 1.0f },
		{ -1.0f, -1.0f,  1.0f, 1.0f }
	};

	// Transform NDC corners to world space
	for (const auto& ndcCorner : ndcCorners) {
		glm::vec4 worldCorner = viewProjectionInverse * ndcCorner;
		corners.push_back(glm::vec3(worldCorner) / worldCorner.w);
	}
	glm::vec3 frustumCenter = viewProjectionInverse * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Transform frustum corners to light's view space and find bounds
	for (const auto& corner : corners) {
		minX = std::min(minX, corner.x);
		maxX = std::max(maxX, corner.x);
		minY = std::min(minY, corner.y);
		maxY = std::max(maxY, corner.y);
		minZ = std::min(minZ, corner.z);
		maxZ = std::max(maxZ, corner.z);
	}

	glm::vec3 origin = { 0.0f, 0.0f, 0.0f };
	if (light->direction.x < 0) {
		origin.x = maxX;
	}
	else {
		origin.x = minX;
	}
	if (light->direction.y < 0) {
		origin.y = maxY;
	}
	else {
		origin.y = minY;
	}
	if (light->direction.z < 0) {
		origin.z = maxZ;
	}
	else {
		origin.z = minZ;
	}

	glm::vec3 V = frustumCenter - origin;
	float VoDir = glm::dot(V, light->direction);
	//Zawiera niw¹ pozycjê œwiat³a
	origin = frustumCenter - VoDir * light->direction;

	//glm::mat4 viewMatrix = glm::lookAt(light->position, light->position + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 viewMatrix = glm::lookAt(origin, origin + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 viewMatrix = glm::lookAt(origin, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

	for (const auto& corner : corners) {
		glm::vec3 transformedCorner = glm::vec3(viewMatrix * glm::vec4(corner, 1.0f));
		minX = std::min(minX, corner.x);
		maxX = std::max(maxX, corner.x);
		minY = std::min(minY, corner.y);
		maxY = std::max(maxY, corner.y);
		minZ = std::min(minZ, corner.z);
		maxZ = std::max(maxZ, corner.z);
	}

	// Adjust minZ and maxZ for better precision in the depth buffer
	float lightMargin = 10.0f; // Adjust based on scene size
	minZ -= lightMargin;
	maxZ += lightMargin;

	light->lightSpaceMatrix = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ) * viewMatrix;/**/

	/*/
	glm::vec3 origin = {10.0f, 10.0f, 0.0f};
	glm::mat4 lightProjection, lightView;
	float near_plane = 1.0f, far_plane = 20.0f;
	//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(origin, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	light->lightSpaceMatrix = lightProjection * lightView;/**/

	//Twin2Engine::Manager::MeshRenderingManager::RenderDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT, light->shadowMapFBO, light->shadowMap, light->lightSpaceMatrix);

	//SPDLOG_INFO("Rekalkulacja LightSpaceM dla dirL");

	return std::move(lightNewPos);
}


void LightingController::RenderShadowMaps() {
	glCullFace(GL_FRONT);

	int i = 0;
	for (auto light : dirLights) {
		Twin2Engine::Manager::MeshRenderingManager::RenderDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT, light->shadowMapFBO, light->shadowMap, light->lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0 + 8 + i);
		glBindTexture(GL_TEXTURE_2D, light->shadowMap);

		++i;
	}

	glCullFace(GL_BACK);
}

void LightingController::SetAmbientLight(glm::vec3 ambientLightColor) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, &ambientLightColor);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingController::SetViewerPosition(glm::vec3& viewerPosition) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &viewerPosition);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//SPDLOG_INFO("SetViewerPosition ({}, {}, {})", viewerPosition.x, viewerPosition.y, viewerPosition.z);

	ViewerTransformChanged.Invoke();
}


void LightingController::SetHighlightParam(float highlightParam) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 28, 4, &highlightParam);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
/*/
void LightingController::SetGamma(float gamma) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 28, 4, &gamma);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
/**/

/*/
void LightingController::RegisterPointLight(PointLight* pointLight) {
	pointLights.insert(pointLight);
}

void LightingController::RegisterSpotLight(SpotLight* spotLight) {
	spotLights.insert(spotLight);
}

void LightingController::RegisterDirLight(DirectionalLight* dirLight) {
	dirLights.insert(dirLight);
}


void LightingController::UnregisterPointLight(PointLight* pointLight) {
	pointLights.erase(pointLight);
}

void LightingController::UnregisterSpotLight(SpotLight* spotLight) {
	spotLights.erase(spotLight);
}

void LightingController::UnregisterDirLight(DirectionalLight* dirLight) {
	dirLights.erase(dirLight);
}/**/
