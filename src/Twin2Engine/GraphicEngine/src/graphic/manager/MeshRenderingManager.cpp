#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/InstatiatingMesh.h>
#include <graphic/Window.h>

#define USE_NAMED_BUFFER_SUBDATA 0

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_renderQueue = std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>();
std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_depthMapRenderQueue = std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>();
std::map<InstatiatingMesh*, std::queue<MeshRenderData>> MeshRenderingManager::_depthQueue;

GLuint MeshRenderingManager::_instanceDataSSBO = 0u;
GLuint MeshRenderingManager::_materialIndexSSBO = 0u;
GLuint MeshRenderingManager::_materialInputUBO = 0u;

void MeshRenderingManager::Init()
{
	// Tworzenie SSBO instanceData
	glGenBuffers(1, &_instanceDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
	// Initialization of buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT_INSTANCE_DATA, _instanceDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		SPDLOG_ERROR("Error1: {}", error);
	};
	
	// Tworzenie SSBO materialIndex
	glGenBuffers(1, &_materialIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
	// Initialization of buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT_MATERIAL_INDEX, _materialIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	// Tworzenie UBO materialInput
	glGenBuffers(1, &_materialInputUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
	// Initialization of buffer
	glBufferData(GL_UNIFORM_BUFFER, MAX_MATERIAL_SIZE * MAX_MATERIAL_NUMBER_PER_DRAW, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, _materialInputUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MeshRenderingManager::UnloadAll()
{
	glDeleteBuffers(1, &_instanceDataSSBO);
	glDeleteBuffers(1, &_materialIndexSSBO);
	glDeleteBuffers(1, &_materialInputUBO);

	_renderQueue.clear();
	_depthQueue.clear();
	_depthMapRenderQueue.clear();
}


void MeshRenderingManager::Render(MeshRenderData meshData)
{
	//SPDLOG_INFO("Trying saving to rendering data1!");
	for (int i = 0; i < meshData.meshes.size(); i++)
	{
		//SPDLOG_INFO("Trying saving to rendering data! {}, {}", meshRenderer->GetMesh(i) != nullptr, meshRenderer->GetMaterial(i) != nullptr);
		if (meshData.meshes[i] != nullptr && meshData.materials[i] != nullptr)
		{
			//SPDLOG_INFO("Saving to rendering data!");
			_renderQueue[meshData.meshes[i]]
				[meshData.materials[i].GetShader()]
				[meshData.materials[i]]
				.push(meshData);

			if (!meshData.isTransparent) {
				_depthMapRenderQueue[meshData.meshes[i]]
					[meshData.materials[i].GetShader()]
					[meshData.materials[i]]
					.push(meshData);

					_depthQueue[meshData.meshes[i]].push(meshData);
			}
		}
		else if (meshData.meshes[i] != nullptr) {
			SPDLOG_WARN("Mesh was null");
		}
		else if (meshData.materials[i] != nullptr) {
			SPDLOG_WARN("Material was null");
		}
	}
}

void MeshRenderingManager::Render()
{
	for (auto& meshPair : _renderQueue)
	{
		for (auto& shaderPair : meshPair.second)
		{

			unsigned int count = 0;

			for (auto& material : shaderPair.second)
			{
				count += material.second.size();
			}

			std::vector<glm::mat4> transforms(count);
			std::vector<unsigned int> indexes(count);
			std::vector<InstanceData> instanceData(count);

			unsigned int index = 0;
			unsigned int materialIndex = 0;

			//std::vector<char> materialData(0);

			size_t size = 0;

			for (auto& material : shaderPair.second)
			{
				const auto& data = material.first.GetMaterialParameters()->GetData();

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
#else
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
				glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
#endif
				size += data.size();

				while (material.second.size() > 0) {
					auto& renderData = material.second.front();

					transforms[index] = renderData.transform;
					indexes[index] = materialIndex;

					++index;

					material.second.pop();
				}

				materialIndex++;
			}

			shaderPair.first->Use();

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			for (auto& material : shaderPair.second)
			{
				material.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);
			}

			size_t instanceIndex = 0;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#endif

				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}

#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			meshPair.first->Draw(count);

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("Error: {}", error);
			}
		}
	}
}

void MeshRenderingManager::RenderDepthMap()
{
	for (auto& meshPair : _depthMapRenderQueue)
	{
		for (auto& shaderPair : meshPair.second)
		{

			unsigned int count = 0;

			for (auto& material : shaderPair.second)
			{
				count += material.second.size();
			}

			std::vector<glm::mat4> transforms(count);
			std::vector<unsigned int> indexes(count);
			std::vector<InstanceData> instanceData(count);

			unsigned int index = 0;
			unsigned int materialIndex = 0;

			size_t size = 0;

			for (auto& material : shaderPair.second)
			{
				const auto& data = material.first.GetMaterialParameters()->GetData();

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
#else
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
				glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
#endif
				size += data.size();

				while (material.second.size() > 0) {
					auto& renderData = material.second.front();

					transforms[index] = renderData.transform;
					indexes[index] = materialIndex;

					instanceData[index].transformMatrix = renderData.transform;
					instanceData[index].materialInputId = materialIndex;

					++index;

					material.second.pop();
				}

				materialIndex++;
			}

			shaderPair.first->Use();

			size_t instanceIndex = 0;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#endif
				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}

#if USE_NAMED_BUFFER_SUBDATA

			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4)* count, transforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int)* count, indexes.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4)* count, transforms.data() + instanceIndex);

			{GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("RDMError2: {}", error);
			}}
			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int)* count, indexes.data() + instanceIndex);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			{GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("RDMError3: {}", error);
			}}
			meshPair.first->Draw(count);

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("RDMError: {}", error);
			}
		}
	}
}

void MeshRenderingManager::RenderDepthMap(const unsigned int& bufferWidth, const unsigned int& bufferHeight, const GLuint& depthFBO, const GLuint& depthMapTex,
	glm::mat4& projectionViewMatrix)
{
	ShaderManager::DepthShader->Use();
	ShaderManager::DepthShader->SetMat4("lightSpaceMatrix", projectionViewMatrix);

	glViewport(0, 0, bufferWidth, bufferHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	//glBindTexture(GL_TEXTURE_2D, depthMapTex);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (auto& meshPair : _depthQueue)
	{
		unsigned int index = 0;

		std::vector<glm::mat4> transforms(meshPair.second.size());

		while (meshPair.second.size() > 0)
		{
			auto& renderData = meshPair.second.front();

			transforms[index] = renderData.transform;

			++index;

			meshPair.second.pop();
		}

		size_t instanceIndex = 0;

		while (index > MAX_INSTANCE_NUMBER_PER_DRAW)
		{
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
#endif
			meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

			instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
			index -= MAX_INSTANCE_NUMBER_PER_DRAW;
		}
#if USE_NAMED_BUFFER_SUBDATA
		//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
		glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4)* index, transforms.data() + instanceIndex);
#else
		//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4)* index, transforms.data() + instanceIndex);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

		meshPair.first->Draw(index);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glm::ivec2 wSize = Twin2Engine::GraphicEngine::Window::GetInstance()->GetContentSize();
	glViewport(0, 0, wSize.x, wSize.y);
}

void MeshRenderingManager::RenderLightingMap(const GLuint& lightingMapFBO) 
{
	ShaderManager::DepthShader->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, lightingMapFBO);

	//glBindTexture(GL_TEXTURE_2D, depthMapTex);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (auto& meshPair : _depthQueue)
	{
		unsigned int index = 0;

		std::vector<glm::mat4> transforms(meshPair.second.size());

		while (meshPair.second.size() > 0)
		{
			auto& renderData = meshPair.second.front();

			transforms[index] = renderData.transform;

			++index;

			meshPair.second.pop();
		}

		size_t instanceIndex = 0;

		while (index > MAX_INSTANCE_NUMBER_PER_DRAW)
		{
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
#endif
			meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

			instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
			index -= MAX_INSTANCE_NUMBER_PER_DRAW;
		}
#if USE_NAMED_BUFFER_SUBDATA
		//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
		glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * index, transforms.data() + instanceIndex);
#else
		//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * index, transforms.data() + instanceIndex);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

		meshPair.first->Draw(index);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}