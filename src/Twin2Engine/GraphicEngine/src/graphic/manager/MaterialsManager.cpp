#include <graphic/manager/MaterialsManager.h>
#include <graphic/LightingController.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace glm;

std::hash<std::string> MaterialsManager::stringHash;
std::map<size_t, MaterialData*> MaterialsManager::loadedMaterials;

std::map<size_t, std::string> MaterialsManager::materialsPaths;

#define TYPE_MAP_INT_HANDLE			0
#define TYPE_MAP_UINT_HANDLE		1
#define TYPE_MAP_FLOAT_HANDLE		2
#define TYPE_MAP_DOUBLE_HANDLE		3
#define TYPE_MAP_BOOL_HANDLE		4
#define TYPE_MAP_TEXTURE2D_HANDLE	5
#define TYPE_MAP_VEC2_HANDLE		6
#define TYPE_MAP_VEC3_HANDLE		7
#define TYPE_MAP_VEC4_HANDLE		8
#define TYPE_MAP_IVEC2_HANDLE		9
#define TYPE_MAP_IVEC3_HANDLE		10
#define TYPE_MAP_IVEC4_HANDLE		11
#define TYPE_MAP_MAT2_HANDLE		12
#define TYPE_MAP_MAT3_HANDLE		13
#define TYPE_MAP_MAT4_HANDLE		14

const std::unordered_map<size_t, int> MaterialsManager::typeHandleMap
{
	{ MaterialsManager::stringHash("int"),			TYPE_MAP_INT_HANDLE			},
	{ MaterialsManager::stringHash("uint"),			TYPE_MAP_UINT_HANDLE		},
	{ MaterialsManager::stringHash("float"),		TYPE_MAP_FLOAT_HANDLE		},
	{ MaterialsManager::stringHash("double"),		TYPE_MAP_DOUBLE_HANDLE		},
	{ MaterialsManager::stringHash("bool"),			TYPE_MAP_BOOL_HANDLE		},
	{ MaterialsManager::stringHash("texture2D"),	TYPE_MAP_TEXTURE2D_HANDLE	},
	{ MaterialsManager::stringHash("vec2"),			TYPE_MAP_VEC2_HANDLE		},
	{ MaterialsManager::stringHash("vec3"),			TYPE_MAP_VEC3_HANDLE		},
	{ MaterialsManager::stringHash("vec4"),			TYPE_MAP_VEC4_HANDLE		},
	{ MaterialsManager::stringHash("ivec2"),		TYPE_MAP_IVEC2_HANDLE		},
	{ MaterialsManager::stringHash("ivec3"),		TYPE_MAP_IVEC3_HANDLE		},
	{ MaterialsManager::stringHash("ivec4"),		TYPE_MAP_IVEC4_HANDLE		},
	{ MaterialsManager::stringHash("mat2"),			TYPE_MAP_MAT2_HANDLE		},
	{ MaterialsManager::stringHash("mat3"),			TYPE_MAP_MAT3_HANDLE		},
	{ MaterialsManager::stringHash("mat4"),			TYPE_MAP_MAT4_HANDLE		}
};

void MaterialsManager::UnloadMaterial(size_t managerId) {

	if (loadedMaterials.find(managerId) != loadedMaterials.end()) {
		MaterialData* matData = loadedMaterials[managerId];
		delete matData->materialParameters;
		delete matData;
		loadedMaterials.erase(managerId);
		materialsPaths.erase(managerId);
	}
}

void MaterialsManager::UnloadMaterial(const std::string& path) {
	UnloadMaterial(stringHash(path));
}

Material MaterialsManager::GetMaterial(size_t managerId)
{
	if (loadedMaterials.find(managerId) != loadedMaterials.end())
	{
		return Material(loadedMaterials[managerId]);
	}
	return Material();
}

Material MaterialsManager::GetMaterial(const std::string& name)
{
	size_t hashed = stringHash(name);

	if (loadedMaterials.find(hashed) != loadedMaterials.end())
	{
		return Material(loadedMaterials[hashed]);
	}
	return LoadMaterial(name);
}

Material MaterialsManager::LoadMaterial(const std::string& materialName)
{
	std::string materialPath = "res/materials/" + materialName + ".material";
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

	size_t materialNameHash = stringHash(materialName);
	SPDLOG_INFO("Loading material {}: {}!", materialNameHash, materialName);

	const YAML::Node& materialNode = fileNode["material"];


	std::string name = materialNode["name"].as<std::string>();
	std::string shader = materialNode["shader"].as<std::string>();

	//MaterialParameters* materialParameters = new MaterialParameters();
	MaterialParametersBuilder materialParametersBuilder;

	for (const auto& parameterNode : materialNode["parameters"]) {
		std::string parameterName = parameterNode["name"].as<std::string>();
		std::string parameterType = parameterNode["type"].as<std::string>();

		const YAML::Node& parameterValue = parameterNode["value"];

		size_t parameterTypeHash = stringHash(parameterType);

		if (!typeHandleMap.contains(parameterTypeHash))
		{
			SPDLOG_ERROR("Incorrect parameter type of loaded material. Parameter name: {}", parameterName);
			break;
		}
		SPDLOG_INFO("LoadSHPR");

		switch (typeHandleMap.at(parameterTypeHash))
		{
		case TYPE_MAP_INT_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<int>());
			break;

		case TYPE_MAP_UINT_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<unsigned int>());
			break;

		case TYPE_MAP_FLOAT_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<float>());
			break;

		case TYPE_MAP_DOUBLE_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<double>());
			break;

		case TYPE_MAP_BOOL_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<bool>());
			break;

		case TYPE_MAP_TEXTURE2D_HANDLE:
		{
			std::string texturePath = parameterValue.as<std::string>();
			Texture2D* texture = Manager::TextureManager::LoadTexture2D(texturePath);
			materialParametersBuilder.AddTexture2D(parameterName, texture->GetId());
		}
		break;

		case TYPE_MAP_VEC2_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<vec2>());
			break;
		
		case TYPE_MAP_VEC3_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<vec3>());
			break;
		
		case TYPE_MAP_VEC4_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<vec4>());
			break;
		
		case TYPE_MAP_IVEC2_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<ivec2>());
			break;
		
		case TYPE_MAP_IVEC3_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<ivec3>());
			break;
		
		case TYPE_MAP_IVEC4_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<ivec4>());
			break;
		
		//case TYPE_MAP_MAT2_HANDLE:
		//	materialParameters->Add(parameterName, parameterValue.as<glm::mat2>());
		//	break;
		//
		//case TYPE_MAP_MAT3_HANDLE:
		//	materialParameters->Add(parameterName, parameterValue.as<glm::mat3>());
		//	break;
		//
		//case TYPE_MAP_MAT4_HANDLE:
		//	materialParameters->Add(parameterName, parameterValue.as<glm::mat4>());
		//	break;
		}
	}

	MaterialData* materialData = new MaterialData
	{
		.id = materialNameHash,
		.shader = Manager::ShaderManager::GetShaderProgram(shader),
		.materialParameters = materialParametersBuilder.Build()
	};
	materialParametersBuilder.Clear();

	loadedMaterials[materialNameHash] = materialData;
	materialsPaths[materialNameHash] = materialName;

	LightingController::Instance()->BindLightBuffors(materialData->shader);
	
	return Material(materialData);
}

YAML::Node MaterialsManager::Serialize()
{
	YAML::Node materials;
	for (const auto& matPair : materialsPaths) {
		materials.push_back(matPair.second);
	}
	return materials;
}
