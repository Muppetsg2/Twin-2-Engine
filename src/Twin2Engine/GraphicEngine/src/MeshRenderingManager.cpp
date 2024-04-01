#include <MeshRenderingManager.h>

#include <InstatiatingMesh.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::GraphicEngine;

std::map<Twin2Engine::GraphicEngine::InstatiatingMesh*, std::map<Twin2Engine::GraphicEngine::Shader*, std::map<Twin2Engine::GraphicEngine::Material, std::vector<Twin2Engine::GraphicEngine::MeshRenderer*>>>> Twin2Engine::GraphicEngine::MeshRenderingManager::renderingData;

void MeshRenderingManager::Register(MeshRenderer* meshRenderer)
{
	//SPDLOG_INFO("Trying saving to rendering data1!");
	for (int i = 0; i < meshRenderer->GetMeshCount(); i++)
	{
		//SPDLOG_INFO("Trying saving to rendering data! {}, {}", meshRenderer->GetMesh(i) != nullptr, meshRenderer->GetMaterial(i) != nullptr);
		if (meshRenderer->GetMesh(i) != nullptr && meshRenderer->GetMaterial(i) != nullptr)
		{

			//SPDLOG_INFO("Saving to rendering data!");
			renderingData[meshRenderer->GetMesh(i)]
						 [meshRenderer->GetMaterial(i).GetShader()]
						 [meshRenderer->GetMaterial(i)]
						 .push_back(meshRenderer);
		}
	}
}

void Twin2Engine::GraphicEngine::MeshRenderingManager::Unregister(MeshRenderer* meshRenderer)
{
	for (int i = 0; i < meshRenderer->GetMeshCount(); i++)
	{
		auto& shaderMap = renderingData[meshRenderer->GetMesh(i)];
		auto& materialMap = shaderMap[meshRenderer->GetMaterial(i).GetShader()];
		auto& list = materialMap[meshRenderer->GetMaterial(i)];

		auto itr = std::find(list.begin(), list.end(), meshRenderer);

		if (itr != list.end())
		{
			list.erase(itr);
		}

		if (list.size() == 0)
		{
			materialMap.erase(materialMap.find(meshRenderer->GetMaterial(i)));

			if (materialMap.size() == 0)
			{
				shaderMap.erase(shaderMap.find(meshRenderer->GetMaterial(i).GetShader()));

				if (shaderMap.size() == 0)
				{
					renderingData.erase(renderingData.find(meshRenderer->GetMesh(i)));
				}
			}
		}
	}
}

void Twin2Engine::GraphicEngine::MeshRenderingManager::Render()
{
	for (auto& meshPair : renderingData)
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

				for (auto& renderer : material.second)
				{
					transforms[index] = renderer->GetTransform()->GetTransformMatrix();
					indexes[index] = materialIndex;

					instanceData[index].transformMatrix = renderer->GetTransform()->GetTransformMatrix();
					instanceData[index].materialInputId = materialIndex;

					index++;
				}

				materialIndex++;
			}

			//SPDLOG_INFO("Instancing objects: {}!", index);
			//SPDLOG_INFO("Instancing objects: {}!", indexes.at(0));
			//SPDLOG_INFO("Instancing objects: {}!", indexes.at(1));

			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			GLuint ssboId = shaderPair.first->GetInstanceDataSSBO();

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboId);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * index, transforms.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			meshPair.first->SetInstanceDataSSBO(ssboId);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			GLuint materialIndexSSBO = shaderPair.first->GetMaterialIndexSSBO();

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialIndexSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * index, indexes.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			meshPair.first->SetMaterialIndexSSBO(materialIndexSSBO);

			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			GLuint uboId = shaderPair.first->GetMaterialInputUBO();

			glBindBuffer(GL_UNIFORM_BUFFER, uboId);
			glBufferData(GL_UNIFORM_BUFFER, materialData.size(), materialData.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			meshPair.first->SetMaterialInputUBO(uboId);

			shaderPair.first->use();
			//meshPair.first->Draw(shaderPair.first, transforms.size());
			meshPair.first->Draw(shaderPair.first, index);
		}
	}
}