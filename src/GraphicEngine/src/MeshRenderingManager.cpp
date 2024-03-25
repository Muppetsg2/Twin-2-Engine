#include <MeshRenderingManager.h>

#include <InstatiatingMesh.h>

#include <spdlog/spdlog.h>

using namespace GraphicEngine;

std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::vector<GraphicEngine::MeshRenderer*>>>> GraphicEngine::MeshRenderingManager::renderingData;

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

void GraphicEngine::MeshRenderingManager::Unregister(MeshRenderer* meshRenderer)
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

void GraphicEngine::MeshRenderingManager::Render()
{
	for (auto& meshPair : renderingData)
	{
		for (auto& shaderPair : meshPair.second)
		{

			GLuint ssboId = shaderPair.first->GetSSBO();
			GLuint uboId = shaderPair.first->GetUBO();

			unsigned int count = 0;

			for (auto material : shaderPair.second)
			{
				count += material.second.size();
			}

			std::vector<glm::mat4> transforms(count);

			unsigned int index = 0;

			for (auto material : shaderPair.second)
			{
				count += material.second.size();

				for (auto renderer : material.second)
				{
					transforms[index] = renderer->getTransform()->GetTransformMatrix();

					index++;
				}
			}

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboId);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 16, transforms.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboId);

			meshPair.first->SetSSBO(ssboId);

			//glBindBuffer(GL_UNIFORM_BUFFER, uboId);
			//glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4, NULL, GL_DYNAMIC_DRAW);
			//glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboId);

			shaderPair.first->use();
			meshPair.first->Draw(shaderPair.first, transforms.size());
		}
	}
}