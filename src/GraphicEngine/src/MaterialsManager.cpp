#include <MaterialsManager.h>

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

			loadedMaterials.erase(found);

			delete material._materialData;
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
		return Material(*found);
	}
	else
	{
		return Material(nullptr);
	}
}

Material MaterialsManager::CreateMaterial(const std::string& newMaterialName, const std::string& shaderName)
{
	size_t hashed = stringHash(newMaterialName);

	std::list<MaterialData*>::iterator found =
		std::find_if(loadedMaterials.begin(), loadedMaterials.end(), [hashed](MaterialData* data) { return data->id == hashed; });

	MaterialData* data;
	if (found == loadedMaterials.end())
	{
		data = new MaterialData{
			.id = hashed,
			.useNumber = 1,
			.shader = ShaderManager::GetShaderProgram(shaderName)
		};

		loadedMaterials.push_back(data);
	}
	else
	{
		data = (*found);
	}
	
	return Material(data);
}