#include <graphic/manager/MaterialsManager.h>
#include <LightingController.h>
#include <filesystem>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;
using namespace glm;

std::hash<std::string> MaterialsManager::_stringHash;
std::map<size_t, Twin2Engine::GraphicEngine::MaterialData*> MaterialsManager::_loadedMaterials;

bool MaterialsManager::_fileDialogOpen = false;
ImFileDialogInfo MaterialsManager::_fileDialogInfo;

std::map<size_t, std::string> MaterialsManager::_materialsPaths;

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

const std::unordered_map<size_t, int> MaterialsManager::_typeHandleMap
{
	{ MaterialsManager::_stringHash("int"),				TYPE_MAP_INT_HANDLE			},
	{ MaterialsManager::_stringHash("uint"),			TYPE_MAP_UINT_HANDLE		},
	{ MaterialsManager::_stringHash("float"),			TYPE_MAP_FLOAT_HANDLE		},
	{ MaterialsManager::_stringHash("double"),			TYPE_MAP_DOUBLE_HANDLE		},
	{ MaterialsManager::_stringHash("bool"),			TYPE_MAP_BOOL_HANDLE		},
	{ MaterialsManager::_stringHash("texture2D"),		TYPE_MAP_TEXTURE2D_HANDLE	},
	{ MaterialsManager::_stringHash("vec2"),			TYPE_MAP_VEC2_HANDLE		},
	{ MaterialsManager::_stringHash("vec3"),			TYPE_MAP_VEC3_HANDLE		},
	{ MaterialsManager::_stringHash("vec4"),			TYPE_MAP_VEC4_HANDLE		},
	{ MaterialsManager::_stringHash("ivec2"),			TYPE_MAP_IVEC2_HANDLE		},
	{ MaterialsManager::_stringHash("ivec3"),			TYPE_MAP_IVEC3_HANDLE		},
	{ MaterialsManager::_stringHash("ivec4"),			TYPE_MAP_IVEC4_HANDLE		},
	{ MaterialsManager::_stringHash("mat2"),			TYPE_MAP_MAT2_HANDLE		},
	{ MaterialsManager::_stringHash("mat3"),			TYPE_MAP_MAT3_HANDLE		},
	{ MaterialsManager::_stringHash("mat4"),			TYPE_MAP_MAT4_HANDLE		}
};

void MaterialsManager::UnloadMaterial(size_t managerId) {

	if (_loadedMaterials.find(managerId) != _loadedMaterials.end()) {
		MaterialData* matData = _loadedMaterials[managerId];
		delete matData->materialParameters;
		delete matData;
		_loadedMaterials.erase(managerId);
		_materialsPaths.erase(managerId);
	}
}

void MaterialsManager::UnloadMaterial(const std::string& path) {
	UnloadMaterial(_stringHash(path));
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

	size_t materialNameHash = _stringHash(materialName);
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

		size_t parameterTypeHash = _stringHash(parameterType);

		if (!_typeHandleMap.contains(parameterTypeHash))
		{
			SPDLOG_ERROR("Incorrect parameter type of loaded material. Parameter name: {}", parameterName);
			break;
		}
		SPDLOG_INFO("LoadSHPR");

		switch (_typeHandleMap.at(parameterTypeHash))
		{
		case TYPE_MAP_INT_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<int>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<int>());
			break;

		case TYPE_MAP_UINT_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<unsigned int>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<unsigned int>());
			break;

		case TYPE_MAP_FLOAT_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<float>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<float>());
			break;

		case TYPE_MAP_DOUBLE_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<double>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<double>());
			break;

		case TYPE_MAP_BOOL_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<bool>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<bool>());
			break;

		case TYPE_MAP_TEXTURE2D_HANDLE:
		{
			std::string texturePath = parameterValue.as<std::string>();
			Texture2D* texture = Manager::TextureManager::LoadTexture2D(texturePath);
			//materialParameters->AddTexture2D(parameterName, texture->GetId());
			materialParametersBuilder.AddTexture2D(parameterName, texture->GetId());
		}
		break;

		case TYPE_MAP_VEC2_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<vec2>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<vec2>());
			break;
		
		case TYPE_MAP_VEC3_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<vec3>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<vec3>());
			break;
		
		case TYPE_MAP_VEC4_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<vec4>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<vec4>());
			break;
		
		case TYPE_MAP_IVEC2_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<ivec2>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<ivec2>());
			break;
		
		case TYPE_MAP_IVEC3_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<ivec3>());
			materialParametersBuilder.Add(parameterName, parameterValue.as<ivec3>());
			break;
		
		case TYPE_MAP_IVEC4_HANDLE:
			//materialParameters->Add(parameterName, parameterValue.as<ivec4>());
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
	//materialParameters->AlignData();

	MaterialData* materialData = new MaterialData
	{
		.id = materialNameHash,
		.shader = Manager::ShaderManager::GetShaderProgram(shader),
		.materialParameters = materialParametersBuilder.Build()
	};
	materialParametersBuilder.Clear();

	_loadedMaterials[materialNameHash] = materialData;
	_materialsPaths[materialNameHash] = materialName;

	LightingSystem::LightingController::Instance()->BindLightBuffors(materialData->shader);
	
	return Material(materialData);
}

//int MaterialsManager::DetermineSize(const std::string& type)
//{
//	return typeSizeMap[stringHash(type)];
//}

/*Material MaterialsManager::CreateMaterial(const std::string& newMaterialName, const std::string& shaderName,
	const std::vector<std::string>& materialParametersNames, const std::vector<unsigned int>& materialParametersSizes, const std::vector<std::string>& textureParametersNames)
{
	size_t hashed = stringHash(newMaterialName);

	MaterialData* data;
	if (loadedMaterials.find(hashed) == loadedMaterials.end())
	{
		SPDLOG_INFO("Creating new material: {}!", newMaterialName);

		data = new MaterialData{
			.id = hashed,
			.shader = ShaderManager::GetShaderProgram(shaderName),
			.materialParameters = new MaterialParameters(materialParametersNames, materialParametersSizes, textureParametersNames)
			
		};

		loadedMaterials[hashed] = data;
	}
	else
	{
		SPDLOG_INFO("Material already exists: {}!", newMaterialName);
		data = loadedMaterials[hashed];
	}
	
	return Material(data);
}*/

Material MaterialsManager::GetMaterial(size_t managerId)
{
	if (_loadedMaterials.find(managerId) != _loadedMaterials.end())
	{
		return Material(_loadedMaterials[managerId]);
	}
	return Material();
}

Material MaterialsManager::GetMaterial(const std::string& name)
{
	size_t hashed = _stringHash(name);

	if (_loadedMaterials.find(hashed) != _loadedMaterials.end())
	{
		return Material(_loadedMaterials[hashed]);
	}
	return LoadMaterial(name);
}

std::string MaterialsManager::GetMaterialName(size_t fontId) {
	if (_materialsPaths.find(fontId) == _materialsPaths.end()) return "";
	std::string p = _materialsPaths[fontId];
	return std::filesystem::path(p).stem().string();
}

std::map<size_t, std::string> MaterialsManager::GetAllMaterialsNames() {
	std::map<size_t, std::string> names = std::map<size_t, std::string>();

	for (auto item : _materialsPaths) {
		names[item.first] = std::filesystem::path(item.second).stem().string();
	}
	return names;
}

YAML::Node MaterialsManager::Serialize()
{
	YAML::Node materials;
	for (const auto& matPair : _materialsPaths) {
		materials.push_back(matPair.second);
	}
	return materials;
}

void MaterialsManager::DrawEditor(bool* p_open)
{
	if (!ImGui::Begin("Materials Manager", p_open)) {
		ImGui::End();
		return;
	}

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(std::string("Materials##Materials Manager").c_str(), node_flag);

	std::list<size_t> clicked = std::list<size_t>();
	clicked.clear();
	if (node_open) {
		int i = 0;
		for (auto& item : _materialsPaths) {
			std::string n = GetMaterialName(item.first);
			ImGui::BulletText(n.c_str());
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
			if (ImGui::Button(std::string("Remove##Materials Manager").append(std::to_string(i)).c_str())) {
				clicked.push_back(item.first);
			}
			++i;
		}
		ImGui::TreePop();
	}

	if (clicked.size() > 0) {
		clicked.sort();

		for (int i = clicked.size() - 1; i > -1; --i)
		{
			UnloadMaterial(clicked.back());

			clicked.pop_back();
		}
	}

	clicked.clear();

	if (ImGui::Button("Load Material##Materials Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		_fileDialogOpen = true;
		_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
		_fileDialogInfo.title = "Open File##Materials Manager";
		_fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\materials");
	}

	if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
	{
		// Result path in: m_fileDialogInfo.resultPath
		LoadMaterial(_fileDialogInfo.resultPath.stem().string());
	}

	ImGui::End();
}