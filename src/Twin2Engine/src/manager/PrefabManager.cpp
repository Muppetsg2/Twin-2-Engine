#include <manager/PrefabManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <core/YamlConverters.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace std;

hash<string> PrefabManager::_hasher;
map<size_t, Prefab*> PrefabManager::_prefabs;
map<size_t, string> PrefabManager::_prefabsPaths;

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
	std::map<size_t, size_t> textures;
	size_t i = 0;
	for (const auto& pathPair : TextureManager::_texturesPaths) {
		Texture2D* tex = TextureManager::_loadedTextures[pathPair.first];

		YAML::Node texNode;
		texNode["path"] = pathPair.second;
		if (TextureManager::_texturesFormats.find(pathPair.first) != TextureManager::_texturesFormats.end()) {
			const auto& formats = TextureManager::_texturesFormats[pathPair.first];
			texNode["fileFormat"] = formats.second;
			texNode["engineFormat"] = formats.first;
		}
		texNode["sWrapMode"] = tex->GetWrapModeS();
		texNode["tWrapMode"] = tex->GetWrapModeT();
		texNode["minFilterMode"] = tex->GetMinFilterMode();
		texNode["magFilterMode"] = tex->GetMagFilterMode();

		prefabNode["Textures"].push_back(texNode);
		textures[pathPair.first] = i++;
	}
#pragma endregion
#pragma region SAVING_SPRITES
	for (const auto& spritePair : SpriteManager::_spriteAliases) {
		Sprite* sprite = SpriteManager::_sprites[spritePair.first];

		YAML::Node spriteNode;
		spriteNode["alias"] = spritePair.second;
		spriteNode["texture"] = textures[sprite->GetTexture()->GetManagerId()];

		if (SpriteManager::_spriteLoadData.find(spritePair.first) != SpriteManager::_spriteLoadData.end()) {
			SpriteData data = SpriteManager::_spriteLoadData[spritePair.first];
			spriteNode["x"] = data.x;
			spriteNode["y"] = data.y;
			spriteNode["width"] = data.width;
			spriteNode["height"] = data.height;
		}

		prefabNode["Sprites"].push_back(spriteNode);
	}
#pragma endregion
#pragma region SAVING_FONTS
	for (const auto& fontPair : FontManager::_fontsPaths) {
		prefabNode["Fonts"].push_back(fontPair.second);
	}
#pragma endregion
#pragma region SAVING_AUDIOS
	for (const auto& audioPair : AudioManager::_audiosPaths) {
		prefabNode["Audio"].push_back(audioPair.second);
	}
#pragma endregion
#pragma region SAVING_MATERIALS
	for (const auto& matPair : MaterialsManager::materialsPaths) {
		prefabNode["Materials"].push_back(matPair.second);
	}
#pragma endregion
#pragma region SAVING_MODELS
	for (const auto& modelPair : ModelsManager::modelsPaths) {
		prefabNode["Models"].push_back(modelPair.second);
	}
#pragma endregion
#pragma region SAVING_PREFABS
	for (const auto& prefabPair : PrefabManager::_prefabsPaths) {
		prefabNode["Prefabs"].push_back(prefabPair.second);
	}
#pragma endregion
#pragma region SAVING_ROOT_OBJECT
	YAML::Node rootNode = obj->Serialize();
	rootNode.remove("id");
	rootNode.remove("children");
	prefabNode["Root"] = rootNode;
#pragma endregion
#pragma region SAVING_GAMEOBJECTS
	// Zapisywaæ dzieci dzieci
	Transform* rootT = obj->GetTransform();
	for (i = 0; i < rootT->GetChildCount(); ++i) {
		GameObject* obj = rootT->GetChildAt(i)->GetGameObject();
		prefabNode["GameObjects"].push_back(obj->Serialize());
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