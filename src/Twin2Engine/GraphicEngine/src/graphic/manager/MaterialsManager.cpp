#include <graphic/manager/MaterialsManager.h>
#include <LightingController.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

std::hash<std::string> MaterialsManager::stringHash;
std::list<Twin2Engine::GraphicEngine::MaterialData*> MaterialsManager::loadedMaterials;

//sconst td::unordered_map<size_t, int> MaterialsManager::typeSizeMap
//{
//	{ MaterialsManager::stringHash("int"), 4 },
//	{ MaterialsManager::stringHash("uint"), 4 },
//	{ MaterialsManager::stringHash("float"), 4 },
//	{ MaterialsManager::stringHash("double"), 8 },
//	{ MaterialsManager::stringHash("bool"), 1 },
//	{ MaterialsManager::stringHash("vec2"), 8 },
//	{ MaterialsManager::stringHash("vec3"), 12 },
//	{ MaterialsManager::stringHash("vec4"), 16 },
//	{ MaterialsManager::stringHash("ivec2"), 8 },
//	{ MaterialsManager::stringHash("ivec3"), 12 },
//	{ MaterialsManager::stringHash("ivec4"), 16 },
//	{ MaterialsManager::stringHash("mat2"), 16 },
//	{ MaterialsManager::stringHash("mat3"), 36 },
//	{ MaterialsManager::stringHash("mat4"), 64 }
//};

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
		return LoadMaterial(name);
	}

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

	MaterialParameters* materialParameters = new MaterialParameters();

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

		switch (typeHandleMap.at(parameterTypeHash))
		{
		case TYPE_MAP_INT_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<int>());
			//int valueInt = parameterValue.as<int>();
			//materialParameters->Add(parameterName, 4, &valueInt);
			break;

		case TYPE_MAP_UINT_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<unsigned int>());
			//unsigned int valueUint = parameterValue.as<unsigned int>();
			//materialParameters->Add(parameterName, 4, &valueUint);
			break;

		case TYPE_MAP_FLOAT_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<float>());
			//float valueFloat = parameterValue.as<float>();
			//materialParameters->Add(parameterName, 4, &valueFloat);
			break;

		case TYPE_MAP_DOUBLE_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<double>());
			//double valueDouble = parameterValue.as<double>();
			//materialParameters->Add(parameterName, 8, &valueDouble);
			break;

		case TYPE_MAP_BOOL_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<bool>());
			//bool valueBool = parameterValue.as<bool>();
			//materialParameters->Add(parameterName, 4, &valueBool);
			break;

		case TYPE_MAP_TEXTURE2D_HANDLE:
		{
			std::string texturePath = parameterValue.as<std::string>();
			Texture2D* texture = Manager::TextureManager::LoadTexture2D(texturePath);
			materialParameters->AddTexture2D(parameterName, texture->GetId());
		}
		break;

		case TYPE_MAP_VEC2_HANDLE:
			materialParameters->Add(parameterName, 8, parameterValue.as<std::vector<float>>().data());
			break;
		
		case TYPE_MAP_VEC3_HANDLE:
			materialParameters->Add(parameterName, 12, parameterValue.as<std::vector<float>>().data());
			break;
		
		case TYPE_MAP_VEC4_HANDLE:
			materialParameters->Add(parameterName, 16, parameterValue.as<std::vector<float>>().data());
			break;
		
		case TYPE_MAP_IVEC2_HANDLE:
			materialParameters->Add(parameterName, 8, parameterValue.as<std::vector<int>>().data());
			break;
		
		case TYPE_MAP_IVEC3_HANDLE:
			materialParameters->Add(parameterName, 12, parameterValue.as<std::vector<int>>().data());
			break;
		
		case TYPE_MAP_IVEC4_HANDLE:
			materialParameters->Add(parameterName, 16, parameterValue.as<std::vector<int>>().data());
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
		.useNumber = 1,
		.shader = Manager::ShaderManager::GetShaderProgram(shader),
		.materialParameters = materialParameters
	};

	LightingSystem::LightingController::Instance()->BindLightBuffors(materialData->shader);

	return Material(materialData);
}

//int MaterialsManager::DetermineSize(const std::string& type)
//{
//	return typeSizeMap[stringHash(type)];
//}

Material MaterialsManager::CreateMaterial(const std::string& newMaterialName, const std::string& shaderName, const std::vector<std::string>& materialParameters)
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
			.materialParameters = new MaterialParameters(materialParameters)
			
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