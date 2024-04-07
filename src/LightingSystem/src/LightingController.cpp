#include "LightingController.h"

#include <glad/glad.h>  
#include <GLFW/glfw3.h> 

using namespace LightingSystem;

LightingController* LightingController::instance = nullptr;
const int LightingController::SHADOW_WIDTH = 1024;
const int LightingController::SHADOW_HEIGHT = 1024;

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
		++itr;
		++dlNumber;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 8, 4, &dlNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 912, dlNumber * 112, &dLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 912 + pos * sizeof(DirectionalLight), 12, &((*itr)->position)); //972
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 912 + pos * 48, 96, *itr); //972
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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
}


void LightingController::BindLightBuffors(Twin2Engine::GraphicEngine::Shader* shader) {
	GLuint block_index = 0;
	block_index = glGetProgramResourceIndex(shader->shaderProgramID, GL_SHADER_STORAGE_BLOCK, "Lights");
	glShaderStorageBlockBinding(shader->shaderProgramID, block_index, 3);

	block_index = glGetUniformBlockIndex(shader->shaderProgramID, "LightingData");
	glUniformBlockBinding(shader->shaderProgramID, block_index, 4);
}

void LightingController::UpdateShadowMapsTab(Twin2Engine::GraphicEngine::Shader* shader) {
	int i = 0;
	shader->Use();
	for (auto dirLight : dirLights) {
		glUniform1i(i, dirLight->shadowMap);
		++i;
	}
}

void LightingController::UpdateDirLightSpaceMatrix(DirectionalLight* light, const glm::mat4& viewProjectionInverse) {
	glm::mat4 lightMatrix = glm::lookAt(light->position, light->position + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));
	
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

	// Transform frustum corners to light's view space and find bounds
	for (const auto& corner : corners) {
		glm::vec3 transformedCorner = glm::vec3(lightMatrix * glm::vec4(corner, 1.0f));
		minX = std::min(minX, transformedCorner.x);
		maxX = std::max(maxX, transformedCorner.x);
		minY = std::min(minY, transformedCorner.y);
		maxY = std::max(maxY, transformedCorner.y);
		minZ = std::min(minZ, transformedCorner.z);
		maxZ = std::max(maxZ, transformedCorner.z);
	}

	// Adjust minZ and maxZ for better precision in the depth buffer
	float lightMargin = 10.0f; // Adjust based on scene size
	minZ -= lightMargin;
	maxZ += lightMargin;

	lightMatrix = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ) * lightMatrix;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, light->shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//ConfigureShaderAndMatrices();
	//RenderScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightingController::SetAmbientLight(glm::vec3& ambientLightColor) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, &ambientLightColor);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingController::SetViewerPosition(glm::vec3& viewerPosition) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &viewerPosition);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingController::SetGamma(float gamma) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 28, 4, &gamma);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
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
