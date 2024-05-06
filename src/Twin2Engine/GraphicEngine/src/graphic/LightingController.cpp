#include <graphic/LightingController.h>

#include <graphic/manager/MeshRenderingManager.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Tools;

LightingController* LightingController::instance = nullptr;
const int LightingController::SHADOW_WIDTH = 2048;
const int LightingController::SHADOW_HEIGHT = 2048;
float LightingController::DLShadowCastingRange = 15.0f;

const STD140Offsets LightingController::_lightsOffsets{
	STD140Variable<STD140Offsets>("pointLights", PointLightOffsets, MAX_POINT_LIGHTS),
	STD140Variable<STD140Offsets>("spotLights", SpotLightOffsets, MAX_SPOT_LIGHTS),
	STD140Variable<STD140Offsets>("directionalLights", DirectionalLightOffsets, MAX_DIRECTIONAL_LIGHTS),
	STD140Variable<unsigned int>("numberOfPointLights"),
	STD140Variable<unsigned int>("numberOfSpotLights"),
	STD140Variable<unsigned int>("numberOfDirLights")
};

const STD140Offsets LightingController::_lightingDataOffsets{
	STD140Variable<glm::vec3>("ambientLight"),
	STD140Variable<glm::vec3>("viewerPosition"),
	STD140Variable<int>("shadingType")
};

LightingController::LightingController() {
	glGenBuffers(1, &LightsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.GetSize(), NULL, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, LightsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	glGenBuffers(1, &LightingDataBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferData(GL_UNIFORM_BUFFER, _lightingDataOffsets.GetSize(), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, LightingDataBuffer);
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

		instance->SetAmbientLight(glm::vec3(0.0f, 0.0f, 0.0f));
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
	STD140Struct lights(_lightsOffsets);
	// POINT LIGHTS
	unsigned int num = 0;
	auto itr1 = pointLights.begin();
	while (itr1 != pointLights.end() && num < MAX_POINT_LIGHTS) {
		lights.Set("pointLights[" + std::to_string(num) + "]", MakeStruct(**itr1));
		++itr1;
		++num;
	}
	lights.Set("numberOfPointLights", num);
	// SPOT LIGHTS
	num = 0;
	auto itr2 = spotLights.begin();
	while (itr2 != spotLights.end() && num < MAX_SPOT_LIGHTS) {
		lights.Set("spotLights[" + std::to_string(num) + "]", MakeStruct(**itr2));
		++itr2;
		++num;
	}
	lights.Set("numberOfSpotLights", num);
	// DIRECTIONAL LIGHTS
	num = 0;
	auto itr3 = dirLights.begin();
	while (itr3 != dirLights.end() && num < MAX_DIRECTIONAL_LIGHTS) {
		lights.Set("directionalLights[" + std::to_string(num) + "]", MakeStruct(**itr3));
		RecalculateDirLightSpaceMatrix(*itr3, CameraData());
		++itr3;
		++num;
	}
	lights.Set("numberOfDirLights", num);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, lights.GetSize(), lights.GetData().data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdatePointLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int plNumber = 0;
	std::vector<std::vector<char>> pLights;
	auto itr = pointLights.begin();
	while (itr != pointLights.end() && plNumber < MAX_POINT_LIGHTS) {
		pLights.push_back(MakeStruct(**itr).GetData());
		++itr;
		++plNumber;
	}
	STD140Struct pLightsStruct;
	pLightsStruct.Add("pLights", PointLightOffsets, pLights);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("numberOfPointLights"), sizeof(unsigned int), &plNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("pointLights"), pLightsStruct.GetSize(), pLightsStruct.GetData().data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateSpotLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int slNumber = 0;
	std::vector<std::vector<char>> sLights;
	auto itr = spotLights.begin();
	while (itr != spotLights.end() && slNumber < MAX_SPOT_LIGHTS) {
		sLights.push_back(MakeStruct(**itr).GetData());
		++itr;
		++slNumber;
	}
	STD140Struct sLightsStruct;
	sLightsStruct.Add("sLights", SpotLightOffsets, sLights);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("numberOfSpotLights"), sizeof(unsigned int), &slNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("spotLights"), sLightsStruct.GetSize(), sLightsStruct.GetData().data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdateDirLights() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
	unsigned int dlNumber = 0;
	std::vector<std::vector<char>> dLights;
	auto itr = dirLights.begin();
	while (itr != dirLights.end() && dlNumber < MAX_DIRECTIONAL_LIGHTS) {
		dLights.push_back(MakeStruct(**itr).GetData());
		++itr;
		++dlNumber;
	}
	STD140Struct dLightsStruct;
	dLightsStruct.Add("dLights", DirectionalLightOffsets, dLights);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("numberOfDirLights"), sizeof(unsigned int), &dlNumber);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("directionalLights"), dLightsStruct.GetSize(), dLightsStruct.GetData().data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingController::UpdatePLPosition(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	if (pos < MAX_POINT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("pointLights[" + std::to_string(pos) + "].position"), sizeof(glm::vec3), &((*itr)->position));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateSLTransform(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	if (pos < MAX_SPOT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("spotLights[" + std::to_string(pos) + "].position"), sizeof(glm::vec3), &((*itr)->position));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("spotLights[" + std::to_string(pos) + "].direction"), sizeof(glm::vec3), &((*itr)->direction));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateDLTransform(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	if (pos < MAX_DIRECTIONAL_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("directionalLights[" + std::to_string(pos) + "].direction"), sizeof(glm::vec3), &((*itr)->direction));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdatePL(PointLight* pointLight) {
	auto itr = pointLights.find(pointLight);
	int pos = std::distance(pointLights.begin(), itr);

	if (pos < MAX_POINT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		STD140Struct pointLight = MakeStruct(**itr);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("pointLights[" + std::to_string(pos) + "]"), pointLight.GetSize(), pointLight.GetData().data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateSL(SpotLight* spotLight) {
	auto itr = spotLights.find(spotLight);
	int pos = std::distance(spotLights.begin(), itr);

	if (pos < MAX_SPOT_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		STD140Struct spotLight = MakeStruct(**itr);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("spotLights[" + std::to_string(pos) + "]"), spotLight.GetSize(), spotLight.GetData().data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::UpdateDL(DirectionalLight* dirLight) {
	auto itr = dirLights.find(dirLight);
	int pos = std::distance(dirLights.begin(), itr);

	if (pos < MAX_DIRECTIONAL_LIGHTS) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsBuffer);
		STD140Struct dirLight = MakeStruct(**itr);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, _lightsOffsets.Get("directionalLights[" + std::to_string(pos) + "]"), dirLight.GetSize(), dirLight.GetData().data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void LightingController::BindLightBuffors(Shader* shader) {
	GLuint block_index = 0;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, LightsBuffer);

	glBindBufferBase(GL_UNIFORM_BUFFER, 4, LightingDataBuffer);

	std::string str = "DirLightShadowMaps[";
	shader->Use();
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "0]").c_str()), 8);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "1]").c_str()), 9);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "2]").c_str()), 10);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, (str + "3]").c_str()), 11);
}

void LightingController::UpdateShadowMapsTab(Shader* shader) {
	int i = 0;
	shader->Use();
	SPDLOG_INFO("Aktualizacja shadow mapy");

	for (auto dirLight : dirLights) { //DirLightShadowMaps
		glActiveTexture(GL_TEXTURE0 + 8 + i);
		glBindTexture(GL_TEXTURE_2D, dirLight->shadowMap);
		++i;
	}
}

glm::vec3 LightingController::RecalculateDirLightSpaceMatrix(DirectionalLight* light, const CameraData& camera) { //const glm::mat4& viewProjectionInverse
	/**/
	glm::mat4 projectionViewInverse = glm::inverse(camera.projection * camera.view);
	std::vector<glm::vec3> corners;

	glm::vec3 lightNewPos;
	glm::vec3 frustumCenter;

	if (!camera.isPerspective) {
		std::vector<glm::vec4> ndcNearCorners = {
			{ -1.0f,  1.0f, -1.0f, 1.0f },
			{  1.0f,  1.0f, -1.0f, 1.0f },
			{  1.0f, -1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f, -1.0f, 1.0f },
		};
		////do robiæ
	}
	else {
		glm::vec3 camPos = camera.pos;
		corners.push_back(camPos);
		frustumCenter = camPos + camera.front * (DLShadowCastingRange * 0.5f);

		float ndcZPos = DLShadowCastingRange / camera.farPlane;
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
	glBufferSubData(GL_UNIFORM_BUFFER, _lightingDataOffsets.Get("ambientLight"), sizeof(glm::vec3), &ambientLightColor);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingController::SetViewerPosition(glm::vec3& viewerPosition) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, _lightingDataOffsets.Get("viewerPosition"), sizeof(glm::vec3), &viewerPosition);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	ViewerTransformChanged.Invoke();
}

void LightingController::SetShadingType(int type) {
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, _lightingDataOffsets.Get("shadingType"), sizeof(int), &type);
	glBindBuffer(GL_UNIFORM_BUFFER, NULL);
}