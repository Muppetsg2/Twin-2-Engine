#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/InstatiatingMesh.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_renderQueue = std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>();
std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_depthMapRenderQueue = std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>();

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

void MeshRenderingManager::End()
{
	glDeleteBuffers(1, &_instanceDataSSBO);
	glDeleteBuffers(1, &_materialIndexSSBO);
	glDeleteBuffers(1, &_materialInputUBO);
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
			}
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

			std::vector<char> materialData(0);

			for (auto& material : shaderPair.second)
			{
				count += material.second.size();

				auto data = material.first.GetMaterialParameters()->GetData();

				materialData.insert(materialData.end(), data.begin(), data.end());

				while (material.second.size() > 0) {
					auto& renderData = material.second.front();

					transforms[index] = renderData.transform;
					indexes[index] = materialIndex;

					//instanceData[index].transformMatrix = renderData.transform; //
					//instanceData[index].transformMatrix = transforms[index];
					//instanceData[index].materialInputId = materialIndex;

					++index;

					material.second.pop();
				}

				materialIndex++;
			}

			////ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			//glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * (index < MAX_INSTANCE_NUMBER_PER_DRAW ? index : MAX_INSTANCE_NUMBER_PER_DRAW), transforms.data());
			//
			////ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * (index < MAX_INSTANCE_NUMBER_PER_DRAW ? index : MAX_INSTANCE_NUMBER_PER_DRAW), indexes.data());
			//
			////ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//glNamedBufferSubData(_materialInputUBO, 0, sizeof(char) * materialData.size(), materialData.data());
			//
			//shaderPair.first->Use();

			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glNamedBufferSubData(_materialInputUBO, 0, sizeof(char) * materialData.size(), materialData.data());

			shaderPair.first->Use();

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			for (auto& material : shaderPair.second)
			{
				material.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);
			}

			while (count >= MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data());

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data());

				meshPair.first->Draw(index);

				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms.data());

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes.data());

			{GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("Error: {}", error);
			}}

			meshPair.first->Draw(index);
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

			std::vector<char> materialData(0);

			for (auto& material : shaderPair.second)
			{
				auto data = material.first.GetMaterialParameters()->GetData();

				materialData.insert(materialData.end(), data.begin(), data.end());

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

			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glNamedBufferSubData(_materialInputUBO, 0, sizeof(char) * materialData.size(), materialData.data());

			shaderPair.first->Use();

			while (count >= MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data());

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data());

				meshPair.first->Draw(index);

				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms.data());

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes.data());

			meshPair.first->Draw(index);
		}
	}
}