#include <MaterialsManager.h>

#include <spdlog/spdlog.h>

using namespace GraphicEngine;

std::hash<std::string> MaterialsManager::stringHash;
std::list<GraphicEngine::MaterialData*> MaterialsManager::loadedMaterials;

void MaterialsManager::UnloadMaterial(Material& material)
{
	if (material != nullptr)
	{
		material._materialData->useNumber--;

		if (material._materialData->useNumber == 0)
		{
			std::list<MaterialData*>::iterator found =
				std::find_if(loadedMaterials.begin(), loadedMaterials.end(), [material](MaterialData* data) { return data->id == material.GetId(); });

			if (found != loadedMaterials.end())
			{
				loadedMaterials.erase(found);

				delete material._materialData->materialParameters;
				delete material._materialData;
			}

		}

		//material = nullptr;
	}
}

Material MaterialsManager::GetMaterial(const std::string& name)
{
	size_t hashed = stringHash(name);

	std::list<MaterialData*>::iterator found =
		std::find_if(loadedMaterials.begin(), loadedMaterials.end(), [hashed](MaterialData* data) { return data->id == hashed; });


	if (found != loadedMaterials.end())
	{
		(*found)->useNumber++;
		return Material(*found);
	}
	else
	{
		return Material(nullptr);
	}
}

Material MaterialsManager::CreateMaterial(const std::string& newMaterialName, const std::string& shaderName, const std::vector<string>& materialParameters)
{
	size_t hashed = stringHash(newMaterialName);

	std::list<MaterialData*>::iterator found =
		std::find_if(loadedMaterials.begin(), loadedMaterials.end(), [hashed](MaterialData* data) { return data->id == hashed; });

	MaterialData* data;
	if (found == loadedMaterials.end())
	{
		SPDLOG_INFO("Creating new material: {}!", newMaterialName);

		data = new MaterialData{
			.id = hashed,
			.useNumber = 1,
			.shader = ShaderManager::GetShaderProgram(shaderName),
			.materialParameters = new MaterialValues(materialParameters)
			
		};

		loadedMaterials.push_back(data);
	}
	else
	{
		SPDLOG_INFO("Material already exists: {}!", newMaterialName);
		data = (*found);
		data->useNumber++;
	}
	
	return Material(data);
}