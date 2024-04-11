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

vector<size_t> SceneManager::_texturesIds = vector<size_t>();
vector<size_t> SceneManager::_spritesIds = vector<size_t>();
vector<size_t> SceneManager::_fontsIds = vector<size_t>();
vector<size_t> SceneManager::_audiosIds = vector<size_t>();

vector<Material> SceneManager::_materialsHolder = vector<Material>();

vector<size_t> SceneManager::_modelsIds = vector<size_t>();
vector<InstatiatingModel> SceneManager::_modelsHolder = vector<InstatiatingModel>();
vector<size_t> SceneManager::_standardModelsIds = vector<size_t>();
vector<InstatiatingModel> SceneManager::_standardModelsHolder = vector<InstatiatingModel>();
vector<InstatiatingModel*> SceneManager::_allModelsHolder = vector<InstatiatingModel*>();

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

	transform->SetLocalPosition(transformNode["position"].as<glm::vec3>());
	transform->SetLocalScale(transformNode["scale"].as<glm::vec3>());
	transform->SetLocalRotation(transformNode["rotation"].as<glm::vec3>());

	// Components
	for (const YAML::Node& compNode : gameObjectNode["components"]) {
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

GameObject* SceneManager::FindObjectWith(GameObject* obj, const Func<bool, const GameObject*>& predicate)
{
	vector<GameObject*> children;
	Transform* objT = obj->GetTransform();
	for (size_t i = 0; i < objT->GetChildCount(); ++i) {
		GameObject* child = objT->GetChildAt(i)->GetGameObject();
		if (predicate(child)) return child;
		children.push_back(child);
	}

	for (const auto& child : children) {
		GameObject* found = FindObjectWith(child, predicate);
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
		if (modelNode["standard"]) {
			scene->AddModel(modelNode["standard"].as<string>(), true);
		}
		else {
			scene->AddModel(modelNode["path"].as<string>());
		}
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
	vector<size_t> ids;
	for (const auto& mat : _materialsHolder) {
		ids.push_back(mat.GetId());
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, ids);

	// Unloading
	for (size_t m : toLoadToUnload.second) {
		for (size_t i = 0; i < ids.size(); ++i) {
			if (ids[i] == m) {
				ids.erase(ids.begin() + i);
				_materialsHolder.erase(_materialsHolder.begin() + i);
				break;
			}
		}
	}
	// Loading
	for (size_t m : toLoadToUnload.first) {
		Material mat = MaterialsManager::LoadMaterial(paths[m]);
		_materialsHolder.push_back(mat);
	}
#pragma endregion
#pragma region LOADING_PATH_MODELS
	// PATH MODELS
	paths.clear();
	for (const auto& path : sceneToLoad->_models) {
		if (!get<0>(path)) {
			paths.push_back(get<1>(path));
		}
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _modelsIds);

	for (size_t m : toLoadToUnload.second) {
		for (size_t i = 0; i < _modelsIds.size(); ++i) {
			if (_modelsIds[i] == m) {
				_modelsIds.erase(_modelsIds.begin() + i);
				_modelsHolder.erase(_modelsHolder.begin() + i);
				break;
			}
		}
	}
	for (size_t m : toLoadToUnload.first) {
		InstatiatingModel model = ModelsManager::GetModel(paths[m]);
		_modelsHolder.push_back(model);
		_modelsIds.push_back(hash<string>()(paths[m]));
	}
#pragma endregion
#pragma region LOADING_STANDARD_MODELS
	// STANDARD MODELS
	paths.clear();
	for (const auto& path : sceneToLoad->_models) {
		if (get<0>(path)) {
			paths.push_back(get<1>(path));
		}
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _standardModelsIds);

	for (size_t m : toLoadToUnload.second) {
		for (size_t i = 0; i < _standardModelsIds.size(); ++i) {
			if (_standardModelsIds[i] == m) {
				_standardModelsIds.erase(_standardModelsIds.begin() + i);
				_standardModelsHolder.erase(_standardModelsHolder.begin() + i);
				break;
			}
		}
	}
	for (size_t m : toLoadToUnload.first) {
		if (paths[m] == "Cube") {
			InstatiatingModel model = ModelsManager::GetCube();
			_standardModelsHolder.push_back(model);
			_standardModelsIds.push_back(hash<string>()(paths[m]));
		}
		else if (paths[m] == "Plane") {
			InstatiatingModel model = ModelsManager::GetPlane();
			_standardModelsHolder.push_back(model);
			_standardModelsIds.push_back(hash<string>()(paths[m]));
		}
		else if (paths[m] == "Sphere") {
			InstatiatingModel model = ModelsManager::GetSphere();
			_standardModelsHolder.push_back(model);
			_standardModelsIds.push_back(hash<string>()(paths[m]));
		}
		else if (paths[m] == "Piramid") {
			InstatiatingModel model = ModelsManager::GetPiramid();
			_standardModelsHolder.push_back(model);
			_standardModelsIds.push_back(hash<string>()(paths[m]));
		}
	}
#pragma endregion
#pragma region LOADING_GAMEOBJECTS
	// GAME OBJECTS
	if (_rootObject != nullptr)	DeleteGameObject(_rootObject);

	_rootObject = new GameObject();
	vector<GameObject*> gameObjects;
	for (const YAML::Node& gameObjectNode : sceneToLoad->_gameObjects) {
		GameObject* obj = CreateGameObject(gameObjectNode);
		_rootObject->GetTransform()->AddChild(obj->GetTransform());
		gameObjects.push_back(obj);
	}

	// ADDING CHILDREN TO GAME OBJECTS
	for (size_t i = 0; i < gameObjects.size(); ++i) {
		for (const YAML::Node& childNode : sceneToLoad->_gameObjects[i]["children"]) {
			gameObjects[i]->GetTransform()->AddChild(gameObjects[childNode.as<size_t>()]->GetTransform());
		}
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

GameObject* SceneManager::FindObjectWithName(const std::string& name)
{
	if (_rootObject->GetName() == name) return _rootObject;
	return FindObjectWith(_rootObject, [&](const GameObject* obj) -> bool { return obj->GetName() == name; });
}

GameObject* SceneManager::FindObjectWithId(size_t id)
{
	if (_rootObject->Id() == id) return _rootObject;
	return FindObjectWith(_rootObject, [&](const GameObject* obj) -> bool { return obj->Id() == id; });
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

Material SceneManager::GetMaterial(size_t loadIdx)
{
	return _materialsHolder[loadIdx];
}

InstatiatingModel SceneManager::GetModel(size_t loadIdx)
{
	return *_allModelsHolder[loadIdx];
}

void SceneManager::UnloadCurrent()
{
	_currentScene = nullptr;
	DeleteGameObject(_rootObject);
	for (size_t id : _texturesIds) {
		TextureManager::UnloadTexture2D(id);
	}
	for (size_t id : _spritesIds) {
		SpriteManager::UnloadSprite(id);
	}
	for (size_t id : _fontsIds) {
		FontManager::UnloadFont(id);
	}
	for (size_t id : _audiosIds) {
		AudioManager::UnloadAudio(id);
	}
	_materialsHolder.clear();
	_modelsIds.clear();
	_modelsHolder.clear();
	_standardModelsIds.clear();
	_standardModelsHolder.clear();
	_allModelsHolder.clear();
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