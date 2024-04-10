#include <manager/SceneManager.h>
#include <core/RenderableComponent.h>
#include <core/ComponentDeserializer.h>

using namespace Twin2Engine::Manager;
using namespace std;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::UI;

Scene* SceneManager::_currentScene = nullptr;
GameObject* SceneManager::_rootObject = nullptr;

vector<size_t> SceneManager::_texturesIds = vector<size_t>();
vector<size_t> SceneManager::_spritesIds = vector<size_t>();
vector<size_t> SceneManager::_fontsIds = vector<size_t>();
vector<size_t> SceneManager::_audiosIds = vector<size_t>();

map<size_t, Scene*> SceneManager::_loadedScenes = map<size_t, Scene*>();

pair<vector<size_t>, vector<size_t>> SceneManager::GetResourcesToLoadAndUnload(const vector<string> paths, const vector<size_t> loadedHashes)
{
	hash<string> hasher = hash<string>();

	vector<size_t> pathHashes;
	for (auto& path : paths) {
		pathHashes.push_back(hasher(path));
	}

	vector<size_t> toUnload, toLoad;
	for (size_t i = 0; i < paths.size(); ++i) {
		toLoad.push_back(i);
	}

	for (auto& h : loadedHashes) {
		bool hasHash = false;
		for (auto& h2 : pathHashes) {
			if (h2 == h) {
				hasHash = true;
				break;
			}
		}

		if (!hasHash) {
			toUnload.push_back(h);
		}
		else {
			for (size_t i = 0; i < toLoad.size(); ++i) {
				if (pathHashes[toLoad[i]] == h) {
					toLoad.erase(toLoad.begin() + i);
				}
			}
		}
	}

	return pair<vector<size_t>, vector<size_t>>(toLoad, toUnload);
}

GameObject* SceneManager::CreateGameObject(const YAML::Node gameObjectNode)
{
	// Game Object
	GameObject* obj = new GameObject();
	obj->SetName(gameObjectNode["name"].as<string>());
	obj->SetIsStatic(gameObjectNode["isStatic"].as<bool>());
	obj->SetActive(gameObjectNode["isActive"].as<bool>());

	// Transform
	YAML::Node transformNode = gameObjectNode["transform"];
	Transform* transform = obj->GetTransform();

	YAML::Node node = transformNode["position"];
	transform->SetLocalPosition({ node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>() });
	node = transformNode["scale"];
	transform->SetLocalScale({ node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>() });
	node = transformNode["rotation"];
	transform->SetLocalRotation({ node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>() });

	// Components
	vector<YAML::Node> componentsNode = gameObjectNode["components"].as<vector<YAML::Node>>();
	for (const YAML::Node& compNode : componentsNode) {
		string type = compNode["type"].as<string>();
		if (!ComponentDeserializer::HasDeserializer(type)) {
			SPDLOG_ERROR("Component Deselializer for given type '{0}' not found", type);
			continue;
		}
		ComponentDeserializer::GetDeserializer(type)(obj, compNode);
	}

	return obj;
}

void SceneManager::DeleteGameObject(GameObject* obj)
{
	Transform* rootTrans = obj->GetTransform();
	size_t childCount = rootTrans->GetChildCount();
	for (size_t i = 0; i < childCount; ++i) {
		Transform* child = rootTrans->GetChildAt(i);
		GameObject* childObj = child->GetGameObject();
		DeleteGameObject(childObj);
		delete child->GetGameObject();
	}
}

void SceneManager::AddScene(const string& name, Scene* scene)
{
	size_t id = hash<string>()(name);
	if (_loadedScenes.find(id) != _loadedScenes.end()) {
		SPDLOG_WARN("Zastêpowanie Sceny o nazwie '{0}'", name);
	}
	_loadedScenes[id] = scene;
}

void SceneManager::AddScene(const string& name, const string& path)
{
	Scene* scene = new Scene();
	YAML::Node sceneNode = YAML::LoadFile(path);

	// Loading Textures
	vector<string> texturePaths;
	vector<YAML::Node> nodes = sceneNode["Textures"].as<vector<YAML::Node>>();

	for (YAML::Node texNode : nodes) {
		TextureData data{
			.sWrapMode = (TextureWrapMode)texNode["sWrapMode"].as<uint32_t>(),
			.tWrapMode = (TextureWrapMode)texNode["tWrapMode"].as<uint32_t>(),
			.minFilterMode = (TextureFilterMode)texNode["minFilterMode"].as<uint32_t>(),
			.magFilterMode = (TextureFilterMode)texNode["magFilterMode"].as<uint32_t>(),
		};
		string path = texNode["path"].as<string>();
		scene->AddTexture2D(path, data);
		texturePaths.push_back(path);
	}

	// Loading Sprites
	nodes = sceneNode["Sprites"].as<vector<YAML::Node>>();

	for (YAML::Node spriteNode : nodes) {
		if (spriteNode["hasParameters"].as<bool>()) {
			SpriteData data{
				.x = spriteNode["x"].as<uint32_t>(),
				.y = spriteNode["y"].as<uint32_t>(),
				.width = spriteNode["width"].as<uint32_t>(),
				.height = spriteNode["height"].as<uint32_t>()
			};
			scene->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()], data);
		}
		else {
			scene->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()]);
		}
	}

	// Loading Fonts
	nodes = sceneNode["Fonts"].as<vector<YAML::Node>>();

	for (YAML::Node fontNode : nodes) {
		scene->AddFont(fontNode.as<string>());
	}

	// Loading Audio
	nodes = sceneNode["Audio"].as<vector<YAML::Node>>();

	for (YAML::Node audioNode : nodes) {
		scene->AddAudio(audioNode.as<string>());
	}

	// Loading GameObjects
	nodes = sceneNode["GameObjects"].as<vector<YAML::Node>>();

	for (YAML::Node gameObjectNode : nodes) {
		scene->AddGameObject(gameObjectNode);
	}

	AddScene(name, scene);
}

void SceneManager::LoadScene(const string& name)
{
	hash<string> hasher;
	size_t sceneId = hasher(name);
	if (_loadedScenes.find(sceneId) == _loadedScenes.end()) {
		SPDLOG_WARN("Nie znaleziono sceny o nazwie '{0}'", name);
		return;
	}

	Scene* sceneToLoad = _loadedScenes[sceneId];

	// TEXTURES
	vector<string> paths;
	for (auto& path : sceneToLoad->_textures) {
		paths.push_back(path.first);
	}

	auto toLoadToUnload = GetResourcesToLoadAndUnload(paths, _texturesIds);
	
	for (size_t t : toLoadToUnload.second) {
		TextureManager::UnloadTexture2D(t);
		for (size_t i = 0; i < _texturesIds.size(); ++i) {
			if (_texturesIds[i] == t) {
				_texturesIds.erase(_texturesIds.begin() + i);
				break;
			}
		}
	}
	for (size_t t : toLoadToUnload.first) {
		string path = paths[t];
		Texture2D* temp = TextureManager::LoadTexture2D(path, sceneToLoad->_textures[path]);
		if (temp != nullptr) _texturesIds.push_back(temp->GetManagerId());
	}

	// SPRITES
	paths.clear();
	for (auto& path : sceneToLoad->_sprites) {
		paths.push_back(path.first);
	}

	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _spritesIds);

	for (size_t s : toLoadToUnload.second) {
		SpriteManager::UnloadSprite(s);
		for (size_t i = 0; i < _spritesIds.size(); ++i) {
			if (_spritesIds[i] == s) {
				_spritesIds.erase(_spritesIds.begin() + i);
				break;
			}
		}
	}
	for (size_t s : toLoadToUnload.first) {
		string alias = paths[s];
		tuple<string, bool, SpriteData> sData = sceneToLoad->_sprites[alias];
		Sprite* temp = nullptr;
		if (get<1>(sData)) {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData), get<2>(sData));
		}
		else {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData));
		}
		if (temp != nullptr) _spritesIds.push_back(temp->GetManagerId());
	}

	// FONTS
	paths.clear();
	for (auto& path : sceneToLoad->_fonts) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _fontsIds);

	for (size_t f : toLoadToUnload.second) {
		FontManager::UnloadFont(f);
		for (size_t i = 0; i < _fontsIds.size(); ++i) {
			if (_fontsIds[i] == f) {
				_fontsIds.erase(_fontsIds.begin() + i);
				break;
			}
		}
	}
	for (size_t f : toLoadToUnload.first) {
		Font* temp = FontManager::LoadFont(paths[f]);
		if (temp != nullptr) _fontsIds.push_back(temp->GetManagerId());
	}

	// AUDIO
	for (auto& path : sceneToLoad->_audios) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _audiosIds);

	for (size_t a : toLoadToUnload.second) {
		AudioManager::UnloadAudio(a);
		for (size_t i = 0; i < _audiosIds.size(); ++i) {
			if (_audiosIds[i] == a) {
				_audiosIds.erase(_audiosIds.begin() + i);
				break;
			}
		}
	}
	for (size_t a : toLoadToUnload.first) {
		size_t id = AudioManager::LoadAudio(paths[a]);
		if (id != 0) _audiosIds.push_back(id);
	}

	// GAME OBJECTS
	if (_rootObject != nullptr)	DeleteGameObject(_rootObject);

	_rootObject = new GameObject();
	vector<GameObject*> gameObjects;
	for (YAML::Node gameObjectNode : sceneToLoad->_gameObjects) {
		GameObject* obj = CreateGameObject(gameObjectNode);
		Transform* parent = nullptr;
		if (gameObjectNode["transform"]["hasParent"].as<bool>()) {
			parent = gameObjects[gameObjectNode["transform"]["parent"].as<size_t>()]->GetTransform();
		}
		else {
			parent = _rootObject->GetTransform();
		}
		obj->GetTransform()->SetParent(parent);
		gameObjects.push_back(obj);
	}

	_currentScene = sceneToLoad;
}

void SceneManager::UpdateCurrentScene()
{
	if (_rootObject == nullptr) return;
	_rootObject->Update();
}

void SceneManager::RenderCurrentScene()
{
	for (auto& comp : RenderableComponent::_components) {
		comp->Render();
	}
}

void SceneManager::UnloadCurrent()
{
	_currentScene = nullptr;
	_rootObject = nullptr;
	for (size_t id : _texturesIds) {
		TextureManager::UnloadTexture2D(id);
	}
}

void SceneManager::UnloadScene(const std::string& name)
{
	size_t h = hash<string>()(name);
	if (_loadedScenes.find(h) == _loadedScenes.end()) {
		SPDLOG_INFO("Failed to unload scene - Scene '{0}' not found", name);
		return;
	}

	Scene*& scene = _loadedScenes[h];
	if (scene == _currentScene) {
		UnloadCurrent();
	}
	delete scene;
	_loadedScenes.erase(h);
}

void SceneManager::UnloadAll()
{
	UnloadCurrent();
	for (auto& sceneP : _loadedScenes) {
		delete sceneP.second;
	}
	_loadedScenes.clear();
}