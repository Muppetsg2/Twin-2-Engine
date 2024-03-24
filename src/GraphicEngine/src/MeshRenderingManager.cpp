#include <MeshRenderingManager.h>

using namespace GraphicEngine;

std::unordered_map<InstatiatingMesh*, std::unordered_map<Shader*, std::unordered_map<Material, std::vector<MeshRenderer*>>>> MeshRenderingManager::renderingData;

void MeshRenderingManager::Register(MeshRenderer* meshRenderer)
{
	for (int i = 0; i < meshRenderer->GetMeshCount(); i++)
	{
		renderingData[meshRenderer->GetMesh(i)]
					 [meshRenderer->GetMaterial(i).GetShader()]
					 [meshRenderer->GetMaterial(i)]
					 .push_back(meshRenderer);
	}
}

void MeshRenderingManager::Unregister(MeshRenderer* meshRenderer)
{
	for (int i = 0; i < meshRenderer->GetMeshCount(); i++)
	{
		auto& shaderMap = renderingData[meshRenderer->GetMesh(i)];
		auto& materialMap = shaderMap[meshRenderer->GetMaterial(i).GetShader()];
		auto& list = materialMap[meshRenderer->GetMaterial(i)];

		list.erase(std::find(list.begin(), list.end(), meshRenderer));

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

void MeshRenderingManager::Render()
{
	for (auto& meshPair : renderingData)
	{
		for (auto& shaderPair : meshPair.second)
		{
			shaderPair.first->use();

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

			//glBindBuffer(GL_UNIFORM_BUFFER, uboId);
			//glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4, NULL, GL_DYNAMIC_DRAW);
			//glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboId);

			meshPair.first->Draw(shaderPair.first);
		}
	}
}