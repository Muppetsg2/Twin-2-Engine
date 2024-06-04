#include <core/Scene.h>
#include <core/RenderableComponent.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace std;

void Scene::AddGameObject(const YAML::Node& gameObjectNode)
{
	_gameObjects.push_back(gameObjectNode);
}

void Scene::AddTexture2D(size_t id, const string& path, const TextureData& data) {
	_textures[id] = { path, data };
}

void Scene::AddTexture2D(size_t id, const string& path, const TextureFormat& format, const TextureFileFormat& fileFormat, const TextureData& data)
{
	_textures[id] = { path, data };
	_texturesFormats[path] = { format, fileFormat };
}

void Scene::AddSprite(size_t id, const string& name, size_t textureId)
{
	_sprites[id] = tuple<string, size_t, bool, SpriteData>(name, textureId, false, SpriteData());
}

void Scene::AddSprite(size_t id, const string& name, size_t textureId, const SpriteData& data)
{
	_sprites[id] = tuple<string, size_t, bool, SpriteData>(name, textureId, true, data);
}

void Scene::AddFont(size_t id, const string& path)
{
	_fonts[id] = path;
}

void Scene::AddAudio(size_t id, const string& path)
{
	_audios[id] = path;
}

void Scene::AddModel(size_t id, const string& path)
{
	_models[id] = path;
}

void Scene::AddMaterial(size_t id, const string& path)
{
	_materials[id] = path;
}

void Scene::AddPrefab(size_t id, const string& path)
{
	_prefabs[id] = path;
}

void Scene::AddScriptableObject(const string& path)
{
	_scriptableObjects.push_back(path);
}

void Scene::Deserialize(const YAML::Node& sceneNode)
{
#pragma region LOAD_TEXTURES_FROM_SCENE_FILE
	vector<string> texturePaths;
	for (const YAML::Node& texNode : sceneNode["Textures"]) {
		size_t id = texNode["id"].as<size_t>();
		string path = texNode["path"].as<string>();
		TextureData data;
		if (texNode["sWrapMode"] || texNode["tWrapMode"] || texNode["minFilterMode"] || texNode["magFilterMode"])
			data = texNode.as<TextureData>();
		if (texNode["fileFormat"] && texNode["engineFormat"]) {
			AddTexture2D(id, path, texNode["engineFormat"].as<TextureFormat>(), texNode["fileFormat"].as<TextureFileFormat>(), data);
		}
		else {
			if (texNode["fileFormat"] && !texNode["engineFormat"]) {
				SPDLOG_ERROR("Przy podanym parametrze 'fileFormat' brakuje parametru 'engineFormat'");
			}
			else if (!texNode["fileFormat"] && texNode["engineFormat"]) {
				SPDLOG_ERROR("Przy podanym parametrze 'fileFormat' brakuje parametru 'engineFormat'");
			}
			AddTexture2D(id, path, data);
		}
		texturePaths.push_back(path);
	}
#pragma endregion
#pragma region LOAD_SPRITES_FROM_SCENE_FILE
	for (const YAML::Node& spriteNode : sceneNode["Sprites"]) {
		if (spriteNode["x"] && spriteNode["y"] && spriteNode["width"] && spriteNode["height"]) {
			AddSprite(spriteNode["id"].as<size_t>(), spriteNode["alias"].as<string>(), spriteNode["texture"].as<size_t>(), spriteNode.as<SpriteData>());
		}
		else {
			if (spriteNode["x"] || spriteNode["y"] || spriteNode["width"] || spriteNode["height"]) {
				SPDLOG_ERROR("Nie podano wszystkich parametrow poprawnie: x, y, width, height");
			}
			AddSprite(spriteNode["id"].as<size_t>(), spriteNode["alias"].as<string>(), spriteNode["texture"].as<size_t>());
		}
	}
#pragma endregion
#pragma region LOAD_FONTS_FROM_SCENE_FILE
	for (const YAML::Node& fontNode : sceneNode["Fonts"]) {
		AddFont(fontNode["id"].as<size_t>(), fontNode["path"].as<string>());
	}
#pragma endregion
#pragma region LOAD_AUDIO_FROM_SCENE_FILE
	for (const YAML::Node& audioNode : sceneNode["Audio"]) {
		AddAudio(audioNode["id"].as<size_t>(), audioNode["path"].as<string>());
	}
#pragma endregion
#pragma region LOAD_MATERIALS_FROM_SCENE_FILE
	for (const YAML::Node& materialNode : sceneNode["Materials"]) {
		AddMaterial(materialNode["id"].as<size_t>(), materialNode["path"].as<string>());
	}
#pragma endregion
#pragma region LOAD_MODELS_FROM_SCENE_FILE
	for (const YAML::Node& modelNode : sceneNode["Models"]) {
		AddModel(modelNode["id"].as<size_t>(), modelNode["path"].as<string>());
	}
#pragma endregion
#pragma region LOAD_PREFABS_FROM_SCENE_FILE
	for (const YAML::Node& prefabNode : sceneNode["Prefabs"]) {
		AddPrefab(prefabNode["id"].as<size_t>(), prefabNode["path"].as<string>());
	}
#pragma endregion
#pragma region LOAD_GAMEOBJECTS_DATA_FROM_SCENE_FILE
	for (const YAML::Node& gameObjectNode : sceneNode["GameObjects"]) {
		AddGameObject(gameObjectNode);
	}
#pragma endregion
#pragma region LOAD_SCRIPTABLEBJECTS_DATA_FROM_SCENE_FILE
	for (const YAML::Node& scriptableObject : sceneNode["ScriptableObjects"]) {
		AddScriptableObject(scriptableObject["path"].as<string>());
	}
#pragma endregion
}