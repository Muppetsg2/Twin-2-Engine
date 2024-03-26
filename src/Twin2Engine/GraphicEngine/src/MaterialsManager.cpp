#include <MaterialsManager.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::GraphicEngine;

std::hash<std::string> MaterialsManager::stringHash;
std::list<Twin2Engine::GraphicEngine::MaterialData*> MaterialsManager::loadedMaterials;

std::unordered_map<size_t, int> MaterialsManager::typeSizeMap
{
	{ MaterialsManager::stringHash("int"), 4 },
	{ MaterialsManager::stringHash("uint"), 4 },
	{ MaterialsManager::stringHash("float"), 4 },
	{ MaterialsManager::stringHash("double"), 8 },
	{ MaterialsManager::stringHash("bool"), 1 },
	{ MaterialsManager::stringHash("vec2"), 8 },
	{ MaterialsManager::stringHash("vec3"), 12 },
	{ MaterialsManager::stringHash("vec4"), 16 },
	{ MaterialsManager::stringHash("ivec2"), 8 },
	{ MaterialsManager::stringHash("ivec3"), 12 },
	{ MaterialsManager::stringHash("ivec4"), 16 },
	{ MaterialsManager::stringHash("mat2"), 16 },
	{ MaterialsManager::stringHash("mat3"), 36 },
	{ MaterialsManager::stringHash("mat4"), 64 }
};

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

Material MaterialsManager::LoadMaterial(const string& materialPath)
{
	YAML::Node fileNode;
	try {
		std::ifstream fin(materialPath);
		if (!fin) {
			SPDLOG_ERROR("Unable to open material file for reading. File path: {}", materialPath);

			return Material();
		}

		fileNode = YAML::Load(fin);
		fin.close();
	}
	catch (const YAML::Exception& e) {
		SPDLOG_ERROR("Exception occured during reading file: {}. YAML Exception: {}", materialPath, e.what());
		return Material();
	}

	const YAML::Node& materialNode = fileNode["material"];

	std::string name = materialNode["name"].as<std::string>();
	std::string shader = materialNode["shader"].as<std::string>();
	std::vector<std::string> propertiesNames;
	std::vector<std::string> propertiesTypes;
	std::vector<int> propertiesSizes;

	for (const auto& propertyNode : materialNode["properties"]) {
		propertiesNames.push_back(propertyNode["name"].as<std::string>());
		propertiesTypes.push_back(propertyNode["type"].as<std::string>());
		// Determine size based on type
		int size = DetermineSize(propertyNode["type"].as<std::string>());
		propertiesSizes.push_back(size);
	}

	return CreateMaterial(name, shader, propertiesNames);
}

int MaterialsManager::DetermineSize(const std::string& type)
{
	return typeSizeMap[stringHash(type)];
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