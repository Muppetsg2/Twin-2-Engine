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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _instanceDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	// Tworzenie SSBO materialIndex
	glGenBuffers(1, &_materialIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _materialIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	// Tworzenie UBO materialInput
	glGenBuffers(1, &_materialInputUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, _materialInputUBO);
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
			_depthMapRenderQueue[meshData.meshes[i]]
				[meshData.materials[i].GetShader()]
				[meshData.materials[i]]
				.push(meshData);
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

					//transforms[index] = projectionViewMatrix * renderData.transform;
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

			//SPDLOG_INFO("Instancing objects: {}!", index);
			//SPDLOG_INFO("Instancing objects: {}!", indexes.at(0));
			//SPDLOG_INFO("Instancing objects: {}!", indexes.at(1));

			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);

			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * index, transforms.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _instanceDataSSBO);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);

			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * index, indexes.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _materialIndexSSBO);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);

			glBufferData(GL_UNIFORM_BUFFER, sizeof(char) * materialData.size(), materialData.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 2, _materialInputUBO);

			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			shaderPair.first->Use();

			// ASSIGNING TEXTURES
			int beginLocation = 0; // glGetUniformLocation(shaderPair.first->GetProgramId(), "constantZeroTexturePoint");
			//int textureBind = GL_TEXTURE0;
			int textureBind = 0;
			for (auto& material : shaderPair.second)
			{
				material.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);
			}

			//SPDLOG_INFO("Rending!");
			//meshPair.first->Draw(shaderPair.first, transforms.size());
			//meshPair.first->Draw(shaderPair.first, index);
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
				count += material.second.size();

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

			//SPDLOG_INFO("Instancing objects: {}!", index);
			//SPDLOG_INFO("Instancing objects: {}!", indexes.at(0));
			//SPDLOG_INFO("Instancing objects: {}!", indexes.at(1));

			////ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			//GLuint ssboId = shaderPair.first->GetInstanceDataSSBO();
			//
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboId);
			//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * index, transforms.data(), GL_DYNAMIC_DRAW);
			//
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			//meshPair.first->SetInstanceDataSSBO(ssboId);
			//
			////ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			//GLuint materialIndexSSBO = shaderPair.first->GetMaterialIndexSSBO();
			//
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialIndexSSBO);
			//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * index, indexes.data(), GL_DYNAMIC_DRAW);
			//
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			//meshPair.first->SetMaterialIndexSSBO(materialIndexSSBO);
			//
			////ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//GLuint uboId = shaderPair.first->GetMaterialInputUBO();
			//
			//glBindBuffer(GL_UNIFORM_BUFFER, uboId);
			//glBufferData(GL_UNIFORM_BUFFER, sizeof(char) * materialData.size(), materialData.data(), GL_DYNAMIC_DRAW);
			//
			//glBindBuffer(GL_UNIFORM_BUFFER, 0);
			//meshPair.first->SetMaterialInputUBO(uboId);
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);

			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * index, transforms.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _instanceDataSSBO);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);

			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * index, indexes.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _materialIndexSSBO);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);

			glBufferData(GL_UNIFORM_BUFFER, sizeof(char) * materialData.size(), materialData.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 2, _materialInputUBO);

			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			shaderPair.first->Use();
			//meshPair.first->Draw(shaderPair.first, transforms.size());
			//meshPair.first->Draw(shaderPair.first, index);
			meshPair.first->Draw(index);
		}
	}
}