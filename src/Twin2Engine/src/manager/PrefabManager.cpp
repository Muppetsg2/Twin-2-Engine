#include <manager/PrefabManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <manager/SceneManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace std;

hash<string> PrefabManager::_hasher;
map<size_t, Prefab*> PrefabManager::_prefabs;
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

	SPDLOG_INFO("Loading Prefab '{0}'", path);
	Prefab* prefab = new Prefab(pathHash);

	YAML::Node prefabNode = YAML::LoadFile(path);

#pragma region LOAD_TEXTURES_FROM_PREFAB_FILE
	vector<string> texturePaths;
	for (const YAML::Node& texNode : prefabNode["Textures"]) {
		string path = texNode["path"].as<string>();
		TextureData data;
		if (texNode["sWrapMode"] || texNode["tWrapMode"] || texNode["minFilterMode"] || texNode["magFilterMode"])
			data = texNode.as<TextureData>();
		if (texNode["fileFormat"] && texNode["engineFormat"]) {
			prefab->AddTexture2D(path, texNode["engineFormat"].as<TextureFormat>(), texNode["fileFormat"].as<TextureFileFormat>(), data);
		}
		else {
			if (texNode["fileFormat"] && !texNode["engineFormat"]) {
				SPDLOG_ERROR("Przy podanym parametrze 'fileFormat' brakuje parametru 'engineFormat'");
			}
			else if (!texNode["fileFormat"] && texNode["engineFormat"]) {
				SPDLOG_ERROR("Przy podanym parametrze 'fileFormat' brakuje parametru 'engineFormat'");
			}
			prefab->AddTexture2D(path, data);
		}
		texturePaths.push_back(path);
	}
#pragma endregion
#pragma region LOAD_SPRITES_FROM_PREFAB_FILE
	for (const YAML::Node& spriteNode : prefabNode["Sprites"]) {
		if (spriteNode["x"] && spriteNode["y"] && spriteNode["width"] && spriteNode["height"]) {
			prefab->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()], spriteNode.as<SpriteData>());
		}
		else {
			if (spriteNode["x"] || spriteNode["y"] || spriteNode["width"] || spriteNode["height"]) {
				SPDLOG_ERROR("Nie podano wszystkich parametrów poprawnie: x, y, width, height");
			}
			prefab->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()]);
		}
	}
#pragma endregion
#pragma region LOAD_FONTS_FROM_PREFAB_FILE
	for (const YAML::Node& fontNode : prefabNode["Fonts"]) {
		prefab->AddFont(fontNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_AUDIO_FROM_PREFAB_FILE
	for (const YAML::Node& audioNode : prefabNode["Audio"]) {
		prefab->AddAudio(audioNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_MATERIALS_FROM_PREFAB_FILE
	for (const YAML::Node& materialNode : prefabNode["Materials"]) {
		prefab->AddMaterial(materialNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_MODELS_FROM_PREFAB_FILE
	for (const YAML::Node& modelNode : prefabNode["Models"]) {
		prefab->AddModel(modelNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_PREFABS_FROM_PREFAB_FILE
	for (const YAML::Node& subPrefabNode : prefabNode["Prefabs"]) {
		prefab->AddPrefab(subPrefabNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_ROOT_GAMEOBJECT_DATA_FROM_PREFAB_FILE
	prefab->SetRootObject(prefabNode["Root"]);
#pragma endregion
#pragma region LOAD_GAMEOBJECTS_DATA_FROM_PREFAB_FILE
	for (const YAML::Node& gameObjectNode : prefabNode["GameObjects"]) {
		prefab->AddGameObject(gameObjectNode);
	}
#pragma endregion

	_prefabs[pathHash] = prefab;
	_prefabsPaths[pathHash] = path;
	return prefab;
}

Prefab* PrefabManager::GetPrefab(size_t id)
{
	if (_prefabs.find(id) == _prefabs.end()) {
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

	ofstream file{ path };
	file << prefabNode;
	file.close();
}

void PrefabManager::UnloadAll()
{
	for (const auto& prefabPair : _prefabs) {
		delete prefabPair.second;
	}
	_prefabs.clear();
	_prefabsPaths.clear();
}

YAML::Node PrefabManager::Serialize()
{
	YAML::Node prefabs;
	for (const auto& prefabPair : _prefabsPaths) {
		prefabs.push_back(prefabPair.second);
	}
	return prefabs;
}
