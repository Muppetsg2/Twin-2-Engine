#include <manager/PrefabManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <manager/SceneManager.h>
#include <manager/ScriptableObjectManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace std;

hash<string> PrefabManager::_hasher;
map<size_t, Prefab*> PrefabManager::_prefabs;

#if _DEBUG
bool PrefabManager::_fileDialogOpen = false;
ImFileDialogInfo PrefabManager::_fileDialogInfo;
#endif

map<size_t, string> PrefabManager::_prefabsPaths;

void PrefabManager::SaveGameObject(const GameObject* obj, YAML::Node gameObjects)
{
	gameObjects.push_back(obj->Serialize());

	Transform* objT = obj->GetTransform();
	for (size_t i = 0; i < objT->GetChildCount(); ++i) {
		SaveGameObject(objT->GetChildAt(i)->GetGameObject(), gameObjects);
	}
}

void PrefabManager::UnloadPrefab(size_t id)
{
	if (_prefabs.find(id) != _prefabs.end()) {
		delete _prefabs[id];
		_prefabs.erase(id);
		_prefabsPaths.erase(id);
	}
}

void PrefabManager::UnloadPrefab(const string& path)
{
	UnloadPrefab(_hasher(path));
}

Prefab* PrefabManager::LoadPrefab(const string& path)
{
	size_t pathHash = _hasher(path);
	if (_prefabs.find(pathHash) != _prefabs.end()) {
		SPDLOG_WARN("Prefab '{0}' already loaded", path);
		return _prefabs[pathHash];
	}

	if (filesystem::exists(path)) {
		SPDLOG_INFO("Loading Prefab '{0}'", path);
		Prefab* prefab = new Prefab(pathHash);
		prefab->Deserialize(YAML::LoadFile(path));
		_prefabs[pathHash] = prefab;
		_prefabsPaths[pathHash] = path;
		return prefab;
	}
	else {
		SPDLOG_CRITICAL("Prefab file '{0}' not found!", path);
		return nullptr;
	}
}

Prefab* PrefabManager::GetPrefab(size_t id)
{
	if (!_prefabs.contains(id)) {
		SPDLOG_ERROR("Prefab of ID '{0}' not found", id);
		return nullptr;
	}
	return _prefabs[id];
}

Prefab* PrefabManager::GetPrefab(const string& path)
{
	size_t pathHash = _hasher(path);
	Prefab* prefab = GetPrefab(pathHash);
	if (prefab == nullptr) {
		return LoadPrefab(path);
	}
	return prefab;
}

std::string PrefabManager::GetPrefabName(size_t id)
{
	if (_prefabs.count(id) == 0) {
		spdlog::error("PrefabManager::Prefab not found");
		return "";
	}

	string p = _prefabsPaths[id];
	return std::filesystem::path(p).stem().string();
}

std::string PrefabManager::GetPrefabName(const std::string& path)
{
	size_t h = hash<string>{}(path);
	return GetPrefabName(h);
}

string PrefabManager::GetPrefabPath(const Prefab* prefab)
{
	if (prefab != nullptr)
	{
		//size_t id = 0;
		//bool foundId = false;
		//for (const auto& pair : _prefabs)
		//{
		//	if (pair.second == prefab)
		//	{
		//		id = pair.first;
		//		break;
		//	}
		//}
		//if (foundId)
		//{
			if (_prefabsPaths.contains(prefab->_id))
			{
				//return _prefabsPaths[id];
				return _prefabsPaths[prefab->_id];
			}
		//}
	}
	return "";
}

std::map<size_t, string> PrefabManager::GetAllPrefabsNames() {
	std::map<size_t, std::string> names = std::map<size_t, std::string>();

	for (auto item : _prefabsPaths) {
		names[item.first] = std::filesystem::path(item.second).stem().string();
	}
	return names;
}

void PrefabManager::SaveAsPrefab(const GameObject* obj, const std::string& path)
{
	YAML::Node prefabNode;

#pragma region SAVING_TEXTURES
	prefabNode["Textures"] = TextureManager::Serialize();
#pragma endregion
#pragma region SAVING_SPRITES
	prefabNode["Sprites"] = SpriteManager::Serialize();
	for (YAML::Node spriteNode : prefabNode["Sprites"]) {
		spriteNode["texture"] = SceneManager::GetTexture2DSaveIdx(spriteNode["texture"].as<size_t>());
	}
#pragma endregion
#pragma region SAVING_FONTS
	prefabNode["Fonts"] = FontManager::Serialize();
#pragma endregion
#pragma region SAVING_AUDIOS
	prefabNode["Audio"] = AudioManager::Serialize();
#pragma endregion
#pragma region SAVING_MATERIALS
	prefabNode["Materials"] = MaterialsManager::Serialize();
#pragma endregion
#pragma region SAVING_MODELS
	prefabNode["Models"] = ModelsManager::Serialize();
#pragma endregion
#pragma region SAVING_PREFABS
	prefabNode["Prefabs"] = PrefabManager::Serialize();
#pragma endregion
#pragma region SAVING_SCRIPTABLE_OBJECTS
	prefabNode["ScriptableObjects"];
	ScriptableObjectManager::SceneSerializationBegin();
#pragma endregion
#pragma region SAVING_ROOT_OBJECT
	YAML::Node rootNode = obj->Serialize();
	rootNode.remove("id");
	rootNode.remove("children");
	prefabNode["Root"] = rootNode;
#pragma endregion
#pragma region SAVING_GAMEOBJECTS
	Transform* rootT = obj->GetTransform();
	for (size_t i = 0; i < rootT->GetChildCount(); ++i) {
		SaveGameObject(rootT->GetChildAt(i)->GetGameObject(), prefabNode["GameObjects"]);
	}
#pragma endregion

	prefabNode["ScriptableObjects"] = ScriptableObjectManager::Serialize();
	ScriptableObjectManager::SceneSerializationEnd();

	ofstream file{ path };
	file << prefabNode;
	file.close();
}

void PrefabManager::UnloadAll()
{
	for (auto& prefabPair : _prefabs) {
		delete prefabPair.second;
	}
	_prefabs.clear();
	_prefabsPaths.clear();
}

YAML::Node PrefabManager::Serialize()
{
	YAML::Node prefabs;
	size_t id = 0;
	for (const auto& prefabPair : _prefabsPaths) {
		YAML::Node prefab;
		prefab["id"] = id++;
		prefab["path"] = prefabPair.second;
		prefabs.push_back(prefab);
	}
	return prefabs;
}

#if _DEBUG
void PrefabManager::DrawEditor(bool* p_open)
{
	if (!ImGui::Begin("Prefab Manager", p_open)) {
		ImGui::End();
		return;
	}

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(string("Prefabs##Prefab Manager").c_str(), node_flag);

	std::list<size_t> clicked = std::list<size_t>();
	clicked.clear();
	if (node_open) {
		int i = 0;
		for (auto& item : _prefabsPaths) {
			string n = GetPrefabName(item.second);
			ImGui::BulletText(n.c_str());
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
			if (ImGui::Button(string("Remove##Prefab Manager").append(std::to_string(i)).c_str())) {
				clicked.push_back(item.first);
			}
			// TODO: DODAC OPCJE EDIT
			++i;
		}
		ImGui::TreePop();
	}

	if (clicked.size() > 0) {
		clicked.sort();

		for (int i = clicked.size() - 1; i > -1; --i)
		{
			UnloadPrefab(clicked.back());

			clicked.pop_back();
		}
	}

	clicked.clear();

	if (ImGui::Button("Load Prefab##Prefab Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		_fileDialogOpen = true;
		_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
		_fileDialogInfo.title = "Open File##Prefab Manager";
		_fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\prefabs");
	}

	if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
	{
		// Result path in: m_fileDialogInfo.resultPath
		LoadPrefab(_fileDialogInfo.resultPath.string());
	}

	ImGui::End();
}
#endif