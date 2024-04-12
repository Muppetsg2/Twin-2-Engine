#include <manager/SceneManager.h>
#include <core/RenderableComponent.h>
#include <core/ComponentDeserializer.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::Manager;
using namespace std;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::UI;

Scene* SceneManager::_currentScene = nullptr;
GameObject* SceneManager::_rootObject = nullptr;

map<size_t, GameObject*> SceneManager::_gameObjectsById;
map<size_t, Component*> SceneManager::_componentsById;

vector<size_t> SceneManager::_texturesIds;
vector<size_t> SceneManager::_spritesIds;
vector<size_t> SceneManager::_fontsIds;
vector<size_t> SceneManager::_audiosIds;
vector<size_t> SceneManager::_materialsIds;
vector<size_t> SceneManager::_modelsIds;;

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

GameObject* SceneManager::FindObjectBy(GameObject* obj, const Func<bool, const GameObject*>& predicate)
{
	vector<GameObject*> children;
	Transform* objT = obj->GetTransform();
	for (size_t i = 0; i < objT->GetChildCount(); ++i) {
		GameObject* child = objT->GetChildAt(i)->GetGameObject();
		if (predicate(child)) return child;
		children.push_back(child);
	}

	for (const auto& child : children) {
		GameObject* found = FindObjectBy(child, predicate);
		if (found != nullptr) return found;
	}

	return nullptr;
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
	for (const YAML::Node& texNode : sceneNode["Textures"]) {
		string path = texNode["path"].as<string>();
		scene->AddTexture2D(path, texNode.as<TextureData>());
		texturePaths.push_back(path);
	}

	// Loading Sprites
	for (const YAML::Node& spriteNode : sceneNode["Sprites"]) {
		if (spriteNode["hasParameters"].as<bool>()) {
			scene->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()], spriteNode.as<SpriteData>());
		}
		else {
			scene->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()]);
		}
	}

	// Loading Fonts
	for (const YAML::Node& fontNode : sceneNode["Fonts"]) {
		scene->AddFont(fontNode.as<string>());
	}

	// Loading Audio
	for (const YAML::Node& audioNode : sceneNode["Audio"]) {
		scene->AddAudio(audioNode.as<string>());
	}

	// Loading Materials
	for (const YAML::Node& materialNode : sceneNode["Materials"]) {
		scene->AddMaterial(materialNode.as<string>());
	}

	// Loading Models
	for (const YAML::Node& modelNode : sceneNode["Models"]) {
		scene->AddModel(modelNode.as<string>());
	}

	// Loading GameObjects
	for (const YAML::Node& gameObjectNode : sceneNode["GameObjects"]) {
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

#pragma region LOADING_TEXTURES
	// TEXTURES
	vector<string> paths;
	for (const auto& path : sceneToLoad->_textures) {
		paths.push_back(path.first);
	}

	auto toLoadToUnload = GetResourcesToLoadAndUnload(paths, _texturesIds);
	
	// Unloading
	for (size_t t : toLoadToUnload.second) {
		TextureManager::UnloadTexture2D(t);
		for (size_t i = 0; i < _texturesIds.size(); ++i) {
			if (_texturesIds[i] == t) {
				_texturesIds.erase(_texturesIds.begin() + i);
				break;
			}
		}
	}
	// Loading
	for (size_t t : toLoadToUnload.first) {
		string path = paths[t];
		Texture2D* temp = TextureManager::LoadTexture2D(path, sceneToLoad->_textures[path]);
		if (temp != nullptr) _texturesIds.push_back(temp->GetManagerId());
	}
	// Sorting
	vector<size_t> sortedIds;
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _texturesIds.size(); ++j) {
			if (_texturesIds[j] == pathH) {
				sortedIds.push_back(_texturesIds[j]);
				break;
			}
		}
	}
	_texturesIds = sortedIds;
#pragma endregion
#pragma region LOADING_SPRITES
	// SPRITES
	paths.clear();
	for (const auto& path : sceneToLoad->_sprites) {
		paths.push_back(path.first);
	}

	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _spritesIds);
	
	// Unloading
	for (size_t s : toLoadToUnload.second) {
		SpriteManager::UnloadSprite(s);
		for (size_t i = 0; i < _spritesIds.size(); ++i) {
			if (_spritesIds[i] == s) {
				_spritesIds.erase(_spritesIds.begin() + i);
				break;
			}
		}
	}
	// Loading
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
	// Sorting
	sortedIds.clear();
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _spritesIds.size(); ++j) {
			if (_spritesIds[j] == pathH) {
				sortedIds.push_back(_spritesIds[j]);
				break;
			}
		}
	}
	_spritesIds = sortedIds;
#pragma endregion
#pragma region LOADING_FONTS
	// FONTS
	paths.clear();
	for (const auto& path : sceneToLoad->_fonts) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _fontsIds);

	// Unloading
	for (size_t f : toLoadToUnload.second) {
		FontManager::UnloadFont(f);
		for (size_t i = 0; i < _fontsIds.size(); ++i) {
			if (_fontsIds[i] == f) {
				_fontsIds.erase(_fontsIds.begin() + i);
				break;
			}
		}
	}
	// Loading
	for (size_t f : toLoadToUnload.first) {
		Font* temp = FontManager::LoadFont(paths[f]);
		if (temp != nullptr) _fontsIds.push_back(temp->GetManagerId());
	}
	// Sorting
	sortedIds.clear();
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _fontsIds.size(); ++j) {
			if (_fontsIds[j] == pathH) {
				sortedIds.push_back(_fontsIds[j]);
				break;
			}
		}
	}
	_fontsIds = sortedIds;
#pragma endregion
#pragma region LOADING_AUDIO
	// AUDIO
	paths.clear();
	for (const auto& path : sceneToLoad->_audios) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _audiosIds);

	// Unloading
	for (size_t a : toLoadToUnload.second) {
		AudioManager::UnloadAudio(a);
		for (size_t i = 0; i < _audiosIds.size(); ++i) {
			if (_audiosIds[i] == a) {
				_audiosIds.erase(_audiosIds.begin() + i);
				break;
			}
		}
	}
	// Loading
	for (size_t a : toLoadToUnload.first) {
		size_t id = AudioManager::LoadAudio(paths[a]);
		if (id != 0) _audiosIds.push_back(id);
	}
	// Sorting
	sortedIds.clear();
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _audiosIds.size(); ++j) {
			if (_audiosIds[j] == pathH) {
				sortedIds.push_back(_audiosIds[j]);
				break;
			}
		}
	}
	_audiosIds = sortedIds;
#pragma endregion
#pragma region LOADING_MATERIALS
	// MATERIALS
	paths.clear();
	for (const auto& path : sceneToLoad->_materials) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _materialsIds);

	// Unloading
	for (size_t m : toLoadToUnload.second) {
		MaterialsManager::UnloadMaterial(m);
		for (size_t i = 0; i < _materialsIds.size(); ++i) {
			if (_materialsIds[i] == m) {
				_materialsIds.erase(_materialsIds.begin() + i);
				break;
			}
		}
	}
	// Loading
	for (size_t m : toLoadToUnload.first) {
		Material mat = MaterialsManager::LoadMaterial(paths[m]);
		_materialsIds.push_back(mat.GetId());
	}
	// Sorting
	sortedIds.clear();
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _materialsIds.size(); ++j) {
			if (_materialsIds[j] == pathH) {
				sortedIds.push_back(_materialsIds[j]);
				break;
			}
		}
	}
	_materialsIds = sortedIds;
#pragma endregion
#pragma region LOADING_MODELS
	// MODELS
	paths.clear();
	for (const auto& path : sceneToLoad->_models) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _modelsIds);

	// Unloading
	for (size_t m : toLoadToUnload.second) {
		ModelsManager::UnloadModel(m);
		for (size_t i = 0; i < _modelsIds.size(); ++i) {
			if (_modelsIds[i] == m) {
				_modelsIds.erase(_modelsIds.begin() + i);
				break;
			}
		}
	}
	// Loading
	for (size_t m : toLoadToUnload.first) {
		InstatiatingModel model = ModelsManager::LoadModel(paths[m]);
		_modelsIds.push_back(model.GetId());
	}
	// Sorting
	sortedIds.clear();
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _modelsIds.size(); ++j) {
			if (_modelsIds[j] == pathH) {
				sortedIds.push_back(_modelsIds[j]);
				break;
			}
		}
	}
#pragma endregion
#pragma region LOADING_GAMEOBJECTS
	// GAME OBJECTS

	// DESTROY OLD OBJECTS
	if (_rootObject != nullptr) {
		_gameObjectsById.clear();
		_componentsById.clear();
		DeleteGameObject(_rootObject);
	}

	// CREATE NEW OBJECTS WITH TRANSFORMS
	map<size_t, GameObject*> objectByComponentId;

	_rootObject = new GameObject(0);
	for (const YAML::Node& gameObjectNode : sceneToLoad->_gameObjects) {
		size_t id = gameObjectNode["id"].as<size_t>();
		GameObject* obj = new GameObject(id);
		_rootObject->GetTransform()->AddChild(obj->GetTransform());
		_gameObjectsById[id] = obj;

		size_t transformId = gameObjectNode["transform"]["id"].as<size_t>();
		_componentsById[transformId] = obj->GetTransform();
		objectByComponentId[transformId] = obj;
	}

	// LOAD GAMEOBJECTS AND TRASFORMS VALUES
	map<size_t, YAML::Node> componentsNodes;
	for (const YAML::Node& gameObjectNode : sceneToLoad->_gameObjects) {
		// GameObject
		GameObject* obj = _gameObjectsById[gameObjectNode["id"].as<size_t>()];
		obj->SetName(gameObjectNode["name"].as<string>());
		obj->SetIsStatic(gameObjectNode["isStatic"].as<bool>());
		obj->SetActive(gameObjectNode["isActive"].as<bool>());

		// Transform
		const YAML::Node& transformNode = gameObjectNode["transform"];
		Transform* t = obj->GetTransform();
		t->SetEnable(transformNode["enabled"].as<bool>());
		t->SetLocalPosition(transformNode["position"].as<glm::vec3>());
		t->SetLocalScale(transformNode["scale"].as<glm::vec3>());
		t->SetLocalRotation(transformNode["rotation"].as<glm::vec3>());

		// Components Node
		componentsNodes[obj->Id()] = gameObjectNode["components"];

		// Set Children
		for (const YAML::Node& childNode : gameObjectNode["children"]) {
			size_t id = childNode.as<size_t>();
			if (_gameObjectsById.find(id) == _gameObjectsById.end()) {
				SPDLOG_ERROR("Couldn't find gameObject with id: {0}, provided in children list of gameObject {1}", id, obj->Id());
				continue;
			}
			t->AddChild(_gameObjectsById[id]->GetTransform());
		}
	}

	// CREATE AND ADD COMPONENTS TO OBJECTS
	for (const auto& compPair : componentsNodes) {
		GameObject* obj = _gameObjectsById[compPair.first];
		for (const YAML::Node& componentNode : compPair.second) {
			string type = componentNode["type"].as<string>();
			if (!ComponentDeserializer::HasDeserializer(type)) {
				SPDLOG_ERROR("Couldn't find deserializer for component of type '{0}'", type);
				continue;
			}

			// Create Component of type
			Component* comp = ComponentDeserializer::GetComponentFunc(type)();

			size_t id = componentNode["id"].as<size_t>();
			_componentsById[id] = comp;
			objectByComponentId[id] = obj;
		}
	}

	// LOAD COMPONENTS VALUES
	for (const auto& compPair : componentsNodes) {
		GameObject* obj = _gameObjectsById[compPair.first];
		for (const YAML::Node& componentNode : compPair.second) {
			string type = componentNode["type"].as<string>();
			if (!ComponentDeserializer::HasDeserializer(type)) {
				SPDLOG_ERROR("Couldn't find deserializer for component of type '{0}'", type);
				continue;
			}

			Component* comp = _componentsById[componentNode["id"].as<size_t>()];

			// Fill Component with values
			comp->SetEnable(componentNode["enabled"].as<bool>());

			// Fill Component with specialized values
			ComponentDeserializer::GetDeserializeAction(type)(comp, componentNode);

			obj->AddComponent(comp);
		}
	}

	// INIT COMPONENTS
	for (const auto& compPair : _componentsById) {
		compPair.second->Init(objectByComponentId[compPair.first], compPair.first);
	}
#pragma endregion

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
		if (comp->IsEnable() && comp->GetGameObject()->GetActive()) 
			comp->Render();
	}
}

GameObject* SceneManager::GetRootObject()
{
	return _rootObject;
}

GameObject* SceneManager::FindObjectByName(const std::string& name)
{
	return FindObjectBy(_rootObject, [&](const GameObject* obj) -> bool { return obj->GetName() == name; });
}

GameObject* SceneManager::GetGameObjectWithId(size_t id)
{
	GameObject* obj = nullptr;
	if (_gameObjectsById.find(id) != _gameObjectsById.end()) {
		obj = _gameObjectsById[id];
	}
	return obj;
}

Component* SceneManager::GetComponentWithId(size_t id)
{
	Component* comp = nullptr;
	if (_componentsById.find(id) != _componentsById.end()) {
		comp = _componentsById[id];
	}
	return comp;
}

size_t SceneManager::GetTexture2D(size_t loadIdx)
{
	return _texturesIds[loadIdx];
}

size_t SceneManager::GetSprite(size_t loadIdx)
{
	return _spritesIds[loadIdx];
}

size_t SceneManager::GetFont(size_t loadIdx)
{
	return _fontsIds[loadIdx];
}

size_t SceneManager::GetAudio(size_t loadIdx)
{
	return _audiosIds[loadIdx];
}

size_t SceneManager::GetMaterial(size_t loadIdx)
{
	return _materialsIds[loadIdx];
}

size_t SceneManager::GetModel(size_t loadIdx)
{
	return _modelsIds[loadIdx];
}

void SceneManager::UnloadCurrent()
{
	_currentScene = nullptr;
	DeleteGameObject(_rootObject);
	for (size_t id : _texturesIds) {
		TextureManager::UnloadTexture2D(id);
	}
	_texturesIds.clear();
	for (size_t id : _spritesIds) {
		SpriteManager::UnloadSprite(id);
	}
	_spritesIds.clear();
	for (size_t id : _fontsIds) {
		FontManager::UnloadFont(id);
	}
	_fontsIds.clear();
	for (size_t id : _audiosIds) {
		AudioManager::UnloadAudio(id);
	}
	_audiosIds.clear();
	for (size_t id : _modelsIds) {
		ModelsManager::UnloadModel(id);
	}
	_modelsIds.clear();
	for (size_t id : _materialsIds) {
		MaterialsManager::UnloadMaterial(id);
	}
	_materialsIds.clear();
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