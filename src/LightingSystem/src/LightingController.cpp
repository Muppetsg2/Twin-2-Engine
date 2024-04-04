#include "LightingController.h"

#include <glad/glad.h>  
#include <GLFW/glfw3.h> 

using namespace LightingSystem;

LightingController* LightingController::instance = nullptr;

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
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightingData), &lightingData);
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
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 12, plNumber * sizeof(PointLight), &pLights);
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
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 396, slNumber * sizeof(SpotLight), &sLights);
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
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 972, dlNumber * sizeof(DirectionalLight), &dLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void LightingController::UpdatePLPosition(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 12 + pos * sizeof(PointLight), 12, &((*itr)->position));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateSLTransform(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 396 + pos * sizeof(SpotLight), 12, &((*itr)->position));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 396 + pos * sizeof(SpotLight), 28, *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateDLTransform(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 972 + pos * sizeof(DirectionalLight), 12, &((*itr)->position)); //972
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 972 + pos * sizeof(DirectionalLight), 28, *itr); //972
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void LightingController::UpdatePL(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 12 + pos * sizeof(PointLight), sizeof(PointLight), *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateSL(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 396 + pos * sizeof(SpotLight), sizeof(SpotLight), *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateDL(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 972 + pos * sizeof(DirectionalLight), sizeof(DirectionalLight), *itr);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void LightingController::BindLightBuffors(Twin2Engine::GraphicEngine::Shader* shader) {
	GLuint block_index = 0;
	block_index = glGetProgramResourceIndex(shader->shaderProgramID, GL_SHADER_STORAGE_BLOCK, "lights");
	glShaderStorageBlockBinding(shader->shaderProgramID, block_index, 3);

	block_index = glGetUniformBlockIndex(shader->shaderProgramID, "lightsData");
	glUniformBlockBinding(shader->shaderProgramID, block_index, 4);
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

void LightingController::SetGamma(float& gamma) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 4, &gamma);
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
