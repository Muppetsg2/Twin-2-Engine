#include <graphic/manager/MaterialsManager.h>
#include <filesystem>
#include <graphic/LightingController.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace glm;

std::hash<std::string> MaterialsManager::_stringHash;
std::map<size_t, Material*> MaterialsManager::_loadedMaterials;

#if _DEBUG
bool MaterialsManager::_fileDialogOpen = false;
ImFileDialogInfo MaterialsManager::_fileDialogInfo;
#endif

std::map<size_t, std::string> MaterialsManager::_materialsPaths;

#define TYPE_MAP_BOOL_HANDLE		0
#define TYPE_MAP_INT_HANDLE			1
#define TYPE_MAP_UINT_HANDLE		2
#define TYPE_MAP_FLOAT_HANDLE		3
#define TYPE_MAP_DOUBLE_HANDLE		4
#define TYPE_MAP_TEXTURE2D_HANDLE	5
#define TYPE_MAP_BVEC1_HANDLE		6
#define TYPE_MAP_BVEC2_HANDLE		7
#define TYPE_MAP_BVEC3_HANDLE		8
#define TYPE_MAP_BVEC4_HANDLE		9
#define TYPE_MAP_IVEC1_HANDLE		10
#define TYPE_MAP_IVEC2_HANDLE		11
#define TYPE_MAP_IVEC3_HANDLE		12
#define TYPE_MAP_IVEC4_HANDLE		13
#define TYPE_MAP_UVEC1_HANDLE		14
#define TYPE_MAP_UVEC2_HANDLE		15
#define TYPE_MAP_UVEC3_HANDLE		16
#define TYPE_MAP_UVEC4_HANDLE		17
#define TYPE_MAP_VEC1_HANDLE		18
#define TYPE_MAP_VEC2_HANDLE		19
#define TYPE_MAP_VEC3_HANDLE		20
#define TYPE_MAP_VEC4_HANDLE		21
#define TYPE_MAP_DVEC1_HANDLE		22
#define TYPE_MAP_DVEC2_HANDLE		23
#define TYPE_MAP_DVEC3_HANDLE		24
#define TYPE_MAP_DVEC4_HANDLE		25
/*
#define TYPE_MAP_MAT2_HANDLE		26
#define TYPE_MAP_MAT3_HANDLE		27
#define TYPE_MAP_MAT4_HANDLE		28
*/

const std::unordered_map<size_t, int> MaterialsManager::_typeHandleMap
{
	{ MaterialsManager::_stringHash("bool"),			TYPE_MAP_BOOL_HANDLE		},
	{ MaterialsManager::_stringHash("int"),				TYPE_MAP_INT_HANDLE			},
	{ MaterialsManager::_stringHash("uint"),			TYPE_MAP_UINT_HANDLE		},
	{ MaterialsManager::_stringHash("float"),			TYPE_MAP_FLOAT_HANDLE		},
	{ MaterialsManager::_stringHash("double"),			TYPE_MAP_DOUBLE_HANDLE		},
	{ MaterialsManager::_stringHash("texture2D"),		TYPE_MAP_TEXTURE2D_HANDLE	},
	{ MaterialsManager::_stringHash("bvec1"),			TYPE_MAP_BVEC1_HANDLE		},
	{ MaterialsManager::_stringHash("bvec2"),			TYPE_MAP_BVEC2_HANDLE		},
	{ MaterialsManager::_stringHash("bvec3"),			TYPE_MAP_BVEC3_HANDLE		},
	{ MaterialsManager::_stringHash("bvec4"),			TYPE_MAP_BVEC4_HANDLE		},
	{ MaterialsManager::_stringHash("ivec1"),			TYPE_MAP_IVEC1_HANDLE		},
	{ MaterialsManager::_stringHash("ivec2"),			TYPE_MAP_IVEC2_HANDLE		},
	{ MaterialsManager::_stringHash("ivec3"),			TYPE_MAP_IVEC3_HANDLE		},
	{ MaterialsManager::_stringHash("ivec4"),			TYPE_MAP_IVEC4_HANDLE		},
	{ MaterialsManager::_stringHash("uvec1"),			TYPE_MAP_UVEC1_HANDLE		},
	{ MaterialsManager::_stringHash("uvec2"),			TYPE_MAP_UVEC2_HANDLE		},
	{ MaterialsManager::_stringHash("uvec3"),			TYPE_MAP_UVEC3_HANDLE		},
	{ MaterialsManager::_stringHash("uvec4"),			TYPE_MAP_UVEC4_HANDLE		},
	{ MaterialsManager::_stringHash("vec1"),			TYPE_MAP_VEC1_HANDLE		},
	{ MaterialsManager::_stringHash("vec2"),			TYPE_MAP_VEC2_HANDLE		},
	{ MaterialsManager::_stringHash("vec3"),			TYPE_MAP_VEC3_HANDLE		},
	{ MaterialsManager::_stringHash("vec4"),			TYPE_MAP_VEC4_HANDLE		},
	{ MaterialsManager::_stringHash("dvec1"),			TYPE_MAP_DVEC1_HANDLE		},
	{ MaterialsManager::_stringHash("dvec2"),			TYPE_MAP_DVEC2_HANDLE		},
	{ MaterialsManager::_stringHash("dvec3"),			TYPE_MAP_DVEC3_HANDLE		},
	{ MaterialsManager::_stringHash("dvec4"),			TYPE_MAP_DVEC4_HANDLE		}
	/*
	{ MaterialsManager::_stringHash("mat2"),			TYPE_MAP_MAT2_HANDLE		},
	{ MaterialsManager::_stringHash("mat3"),			TYPE_MAP_MAT3_HANDLE		},
	{ MaterialsManager::_stringHash("mat4"),			TYPE_MAP_MAT4_HANDLE		}
	*/
};

void MaterialsManager::UnloadMaterial(size_t managerId) {

	if (_loadedMaterials.contains(managerId)) {
		Material* mat = _loadedMaterials[managerId];
		delete mat;
		_loadedMaterials.erase(managerId);
		_materialsPaths.erase(managerId);
	}
}

void MaterialsManager::UnloadMaterial(const std::string& materialPath) {
	UnloadMaterial(_stringHash(materialPath));
}

Material* MaterialsManager::LoadMaterial(const std::string& materialPath)
{
	size_t hash = _stringHash(materialPath);
	YAML::Node fileNode;
	try {
		std::ifstream fin(materialPath);
		if (!fin) {
			SPDLOG_ERROR("Unable to open material file for reading. File path: {}", materialPath);

			return nullptr;
		}

		fileNode = YAML::Load(fin);
		fin.close();
	}
	catch (const YAML::Exception& e) {
		SPDLOG_ERROR("Exception occured during reading file: {}. YAML Exception: {}", materialPath, e.what());
		return nullptr;
	}

	//size_t materialNameHash = _stringHash(materialName);
	//SPDLOG_INFO("Loading material {}: {}!", materialNameHash, materialName);

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
		SPDLOG_INFO("LoadSHPR {}", materialPath);

		switch (_typeHandleMap.at(parameterTypeHash))
		{
		case TYPE_MAP_BOOL_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<bool>());
			break;

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

		case TYPE_MAP_TEXTURE2D_HANDLE:
		{
			std::string texturePath = parameterValue.as<std::string>();
			Texture2D* texture = Manager::TextureManager::LoadTexture2D(texturePath);
			materialParametersBuilder.AddTexture2D(parameterName, texture->GetId());
			break;
		}
		
		case TYPE_MAP_BVEC1_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<bvec1>());
			break;

		case TYPE_MAP_BVEC2_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<bvec2>());
			break;

		case TYPE_MAP_BVEC3_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<bvec3>());
			break;

		case TYPE_MAP_BVEC4_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<bvec4>());
			break;

		case TYPE_MAP_IVEC1_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<ivec1>());
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

		case TYPE_MAP_UVEC1_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<uvec1>());
			break;

		case TYPE_MAP_UVEC2_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<uvec2>());
			break;

		case TYPE_MAP_UVEC3_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<uvec3>());
			break;

		case TYPE_MAP_UVEC4_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<uvec4>());
			break;

		case TYPE_MAP_VEC1_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<vec1>());
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

		case TYPE_MAP_DVEC1_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<dvec1>());
			break;

		case TYPE_MAP_DVEC2_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<dvec2>());
			break;

		case TYPE_MAP_DVEC3_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<dvec3>());
			break;

		case TYPE_MAP_DVEC4_HANDLE:
			materialParametersBuilder.Add(parameterName, parameterValue.as<dvec4>());
			break;
		
		/*
		case TYPE_MAP_MAT2_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<glm::mat2>());
			break;
		
		case TYPE_MAP_MAT3_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<glm::mat3>());
			break;
		
		case TYPE_MAP_MAT4_HANDLE:
			materialParameters->Add(parameterName, parameterValue.as<glm::mat4>());
			break;
		*/
		}
	}

	MaterialData materialData
	{
		//.id = materialNameHash,
		.id = hash,
		.shader = Manager::ShaderManager::GetShaderProgram(shader),
		.materialParameters = materialParametersBuilder.Build()
	};

	_loadedMaterials[hash] = new Material(materialData);
	_materialsPaths[hash] = materialPath;

#if _DEBUG
	_loadedMaterials[hash]->_materialParameters->_textureNames = materialParametersBuilder.GetParametersNames();
#endif

	materialParametersBuilder.Clear();

	LightingController::Instance()->BindLightBuffors(materialData.shader);
	materialData.shader->Use();
	materialData.shader->SetInt("occlusionMap", 31);
	materialData.shader->SetInt("depthMap", 26);
	
	return _loadedMaterials[hash];
}

#if _DEBUG
void MaterialsManager::SaveMaterial(const std::string& materialPath, Material* mat)
{
	if (!std::filesystem::exists(std::filesystem::path(materialPath).parent_path())) {
		SPDLOG_ERROR("Couldn't save Material. Folder {} doesn't exists!", std::filesystem::path(materialPath).parent_path().string().c_str());
		return;
	}

	if (mat == nullptr) {
		SPDLOG_ERROR("Couldn't save Material. Material was nullptr!");
		return;
	}

	// Write YAML file with material parameters
	YAML::Node materialNode;
	materialNode["name"] = std::filesystem::path(materialPath).stem().string();
	materialNode["shader"] = ShaderManager::GetShaderName(mat->GetShader()->GetProgramId());
	materialNode["parameters"] = mat->_materialParameters->Serialize()["parameters"];

	YAML::Node rootNode;
	rootNode["material"] = materialNode;
	
	// Write YAML file
	std::ofstream yamlFile(materialPath);
	if (!yamlFile.is_open()) {
		SPDLOG_ERROR("Failed to create material file. Path: {}", materialPath);
		return;
	}
	yamlFile << rootNode;
	yamlFile.close();

	SPDLOG_INFO("Material '{}' file saved successfully. Path {}", std::filesystem::path(materialPath).stem().string(), materialPath);
}
#endif

bool MaterialsManager::IsMaterialLoaded(size_t managerId) {
	return _loadedMaterials.contains(managerId);
}

bool MaterialsManager::IsMaterialLoaded(const std::string& materialPath) {
	return IsMaterialLoaded(_stringHash(materialPath));
}

Material* MaterialsManager::GetMaterial(size_t managerId)
{
	if (_loadedMaterials.contains(managerId))
	{
		return _loadedMaterials[managerId];
	}
	return nullptr;
}

Material* MaterialsManager::GetMaterial(const std::string& materialPath)
{
	size_t hashed = _stringHash(materialPath);

	if (_loadedMaterials.contains(hashed))
	{
		return _loadedMaterials[hashed];
	}
	return LoadMaterial(materialPath);
}

std::string MaterialsManager::GetMaterialName(size_t managerId) {
	if (!_materialsPaths.contains(managerId)) return "";
	std::string p = _materialsPaths[managerId];
	return std::filesystem::path(p).stem().string();
}

std::map<size_t, std::string> MaterialsManager::GetAllMaterialsNames() {
	std::map<size_t, std::string> names = std::map<size_t, std::string>();

	for (auto item : _materialsPaths) {
		names[item.first] = std::filesystem::path(item.second).stem().string();
	}
	return names;
}

void MaterialsManager::UnloadAll() 
{
	for (auto item : _loadedMaterials) {
		delete item.second;
	}

	_loadedMaterials.clear();
	//_typeHandleMap.clear();
	_materialsPaths.clear();
}

YAML::Node MaterialsManager::Serialize()
{
	YAML::Node materials;
	size_t id = 0;
	for (const auto& matPair : _materialsPaths) {
		YAML::Node material;
		material["id"] = id++;
		material["path"] = matPair.second;
		materials.push_back(material);
	}
	return materials;
}

#if _DEBUG
void MaterialsManager::DrawEditor(bool* p_open)
{
	if (!ImGui::Begin("Materials Manager", p_open)) {
		ImGui::End();
		return;
	}

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(std::string("Materials##Materials Manager").c_str(), node_flag);

	std::list<size_t> clicked = std::list<size_t>();
	static size_t selectedToEdit = 0;
	static bool openEditor = true;
	clicked.clear();
	if (node_open) {
		int i = 0;
		for (auto& item : _materialsPaths) {
			std::string n = GetMaterialName(item.first);
			ImGui::BulletText(n.c_str());
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50);
			if (ImGui::Button(std::string("Edit##Materials Manager").append(std::to_string(i)).c_str())) {
				selectedToEdit = item.first;
				openEditor = true;
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
			if (ImGui::RemoveButton(std::string("##Remove Materials Manager").append(std::to_string(i)).c_str())) {
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

	if (selectedToEdit != 0) {
		if (ImGui::Begin("Material Editor##Materials Manager", &openEditor)) {
			_loadedMaterials[selectedToEdit]->DrawEditor();

			if (ImGui::Button(std::string("Save##Material").append(std::to_string(selectedToEdit)).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				SaveMaterial(_materialsPaths[selectedToEdit], _loadedMaterials[selectedToEdit]);
			}
		}
		ImGui::End();

		if (!openEditor) {
			selectedToEdit = 0;
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
		LoadMaterial(std::filesystem::relative(_fileDialogInfo.resultPath).string());
	}

	ImGui::End();
}
#endif