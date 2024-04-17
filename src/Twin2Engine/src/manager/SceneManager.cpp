#include <manager/SceneManager.h>
#include <core/RenderableComponent.h>
#include <core/ComponentDeserializer.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <manager/PrefabManager.h>
#include <core/EventHandler.h>

using namespace Twin2Engine::Manager;
using namespace std;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;

size_t SceneManager::_currentSceneId;
std::string SceneManager::_currentSceneName;
GameObject* SceneManager::_rootObject = nullptr;

map<size_t, GameObject*> SceneManager::_gameObjectsById;
map<size_t, Component*> SceneManager::_componentsById;

vector<size_t> SceneManager::_texturesIds;
vector<size_t> SceneManager::_spritesIds;
vector<size_t> SceneManager::_fontsIds;
vector<size_t> SceneManager::_audiosIds;
vector<size_t> SceneManager::_materialsIds;
vector<size_t> SceneManager::_modelsIds;
vector<size_t> SceneManager::_prefabsIds;

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
	Transform* trans = obj->GetTransform();
	size_t childCount = trans->GetChildCount();
	for (size_t i = 0; i < childCount; ++i) {
		Transform* child = trans->GetChildAt(i);
		GameObject* childObj = child->GetGameObject();
		DeleteGameObject(childObj);
		_gameObjectsById.erase(childObj->Id());
		delete childObj;
	}
}

void SceneManager::SaveGameObject(const GameObject* obj, YAML::Node gameObjects)
{
	gameObjects.push_back(obj->Serialize());

	Transform* objT = obj->GetTransform();
	for (size_t i = 0; i < objT->GetChildCount(); ++i) {
		SaveGameObject(objT->GetChildAt(i)->GetGameObject(), gameObjects);
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

#pragma region LOAD_TEXTURES_FROM_SCENE_FILE
	vector<string> texturePaths;
	for (const YAML::Node& texNode : sceneNode["Textures"]) {
		string path = texNode["path"].as<string>();
		TextureData data;
		if (texNode["sWrapMode"] || texNode["tWrapMode"] || texNode["minFilterMode"] || texNode["magFilterMode"])
			data = texNode.as<TextureData>();
		if (texNode["fileFormat"] && texNode["engineFormat"]) {
			scene->AddTexture2D(path, texNode["engineFormat"].as<TextureFormat>(), texNode["fileFormat"].as<TextureFileFormat>(), data);
		}		
		else {
			if (texNode["fileFormat"] && !texNode["engineFormat"]) {
				SPDLOG_ERROR("Przy podanym parametrze 'fileFormat' brakuje parametru 'engineFormat'");
			}
			else if (!texNode["fileFormat"] && texNode["engineFormat"]) {
				SPDLOG_ERROR("Przy podanym parametrze 'fileFormat' brakuje parametru 'engineFormat'");
			}
			scene->AddTexture2D(path, data);
		}
		texturePaths.push_back(path);
	}
#pragma endregion
#pragma region LOAD_SPRITES_FROM_SCENE_FILE
	for (const YAML::Node& spriteNode : sceneNode["Sprites"]) {
		if (spriteNode["x"] && spriteNode["y"] && spriteNode["width"] && spriteNode["height"]) {
			scene->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()], spriteNode.as<SpriteData>());
		}
		else {
			if (spriteNode["x"] || spriteNode["y"] || spriteNode["width"] || spriteNode["height"]) {
				SPDLOG_ERROR("Nie podano wszystkich parametrów poprawnie: x, y, width, height");
			}
			scene->AddSprite(spriteNode["alias"].as<string>(), texturePaths[spriteNode["texture"].as<size_t>()]);
		}
	}
#pragma endregion
#pragma region LOAD_FONTS_FROM_SCENE_FILE
	for (const YAML::Node& fontNode : sceneNode["Fonts"]) {
		scene->AddFont(fontNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_AUDIO_FROM_SCENE_FILE
	for (const YAML::Node& audioNode : sceneNode["Audio"]) {
		scene->AddAudio(audioNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_MATERIALS_FROM_SCENE_FILE
	for (const YAML::Node& materialNode : sceneNode["Materials"]) {
		scene->AddMaterial(materialNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_MODELS_FROM_SCENE_FILE
	for (const YAML::Node& modelNode : sceneNode["Models"]) {
		scene->AddModel(modelNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_PREFABS_FROM_SCENE_FILE
	for (const YAML::Node& prefabNode : sceneNode["Prefabs"]) {
		scene->AddPrefab(prefabNode.as<string>());
	}
#pragma endregion
#pragma region LOAD_GAMEOBJECTS_DATA_FROM_SCENE_FILE
	for (const YAML::Node& gameObjectNode : sceneNode["GameObjects"]) {
		scene->AddGameObject(gameObjectNode);
	}
#pragma endregion

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
		Texture2D* temp = nullptr;
		if (sceneToLoad->_texturesFormats.find(path) != sceneToLoad->_texturesFormats.end()) {
			const auto& formats = sceneToLoad->_texturesFormats[path];
			temp = TextureManager::LoadTexture2D(path, formats.second, formats.first, sceneToLoad->_textures[path]);
		}
		else {
			temp = TextureManager::LoadTexture2D(path, sceneToLoad->_textures[path]);
		}
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
	_modelsIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFABS
	// MODELS
	paths.clear();
	for (const auto& path : sceneToLoad->_prefabs) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _prefabsIds);

	// Unloading
	for (size_t p : toLoadToUnload.second) {
		PrefabManager::UnloadPrefab(p);
		for (size_t i = 0; i < _prefabsIds.size(); ++i) {
			if (_prefabsIds[i] == p) {
				_prefabsIds.erase(_prefabsIds.begin() + i);
				break;
			}
		}
	}
	// Loading
	for (size_t p : toLoadToUnload.first) {
		Prefab* prefab = PrefabManager::LoadPrefab(paths[p]);
		if (prefab == nullptr) {
			SPDLOG_ERROR("Couldn't load prefab {0}", paths[p]);
			continue;
		}
		_prefabsIds.push_back(prefab->GetId());
	}
	// Sorting
	sortedIds.clear();
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _prefabsIds.size(); ++j) {
			if (_prefabsIds[j] == pathH) {
				sortedIds.push_back(_prefabsIds[j]);
				break;
			}
		}
	}
	_prefabsIds = sortedIds;
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
	_gameObjectsById[0] = _rootObject;
	for (const YAML::Node& gameObjectNode : sceneToLoad->_gameObjects) {
		size_t id = gameObjectNode["id"].as<size_t>();
		if (id == 0) {
			SPDLOG_ERROR("Object ID cannot be equal 0");
			continue;
		}
		GameObject* obj = new GameObject(id);
		_rootObject->GetTransform()->AddChild(obj->GetTransform());
		_gameObjectsById[id] = obj;

		size_t transformId = gameObjectNode["transform"]["id"].as<size_t>();
		_componentsById[transformId] = obj->GetTransform();
		objectByComponentId[transformId] = obj;
	}

	// LOAD GAMEOBJECTS AND TRANSFORMS VALUES
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
		t->SetLocalRotation(glm::radians(transformNode["rotation"].as<glm::vec3>()));

		// Components Node
		componentsNodes[obj->Id()] = gameObjectNode["components"];

		// Set Children
		for (const YAML::Node& childNode : gameObjectNode["children"]) {
			size_t id = childNode.as<size_t>();
			if (id == 0) {
				SPDLOG_ERROR("Couldn't set rootObject (id: 0) as Child of gameObject");
				continue;
			}
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

			if (componentNode["subTypes"]) {
				// Fill Component with SubTypes values
				for (const YAML::Node& subTypeNode : componentNode["subTypes"]) {
					string subType = subTypeNode.as<string>();
					if (!ComponentDeserializer::HasDeserializer(subType)) {
						SPDLOG_ERROR("Couldn't find deserializer for component of subType '{0}'", subType);
						continue;
					}

					ComponentDeserializer::GetDeserializeAction(subType)(comp, componentNode);
				}
			}
			

			// Fill Component with Type values
			ComponentDeserializer::GetDeserializeAction(type)(comp, componentNode);

			obj->AddComponent(comp);
		}
	}

	// INIT COMPONENTS
	for (const auto& compPair : _componentsById) {
		compPair.second->Init(objectByComponentId[compPair.first], compPair.first);
	}
#pragma endregion

	_currentSceneName = name;
	_currentSceneId = sceneId;
}

void SceneManager::SaveScene(const string& path) {
	YAML::Node sceneNode;

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

		sceneNode["Textures"].push_back(texNode);
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

		sceneNode["Sprites"].push_back(spriteNode);
	}
#pragma endregion
#pragma region SAVING_FONTS
	for (const auto& fontPair : FontManager::_fontsPaths) {
		sceneNode["Fonts"].push_back(fontPair.second);
	}
#pragma endregion
#pragma region SAVING_AUDIOS
	for (const auto& audioPair : AudioManager::_audiosPaths) {
		sceneNode["Audio"].push_back(audioPair.second);
	}
#pragma endregion
#pragma region SAVING_MATERIALS
	for (const auto& matPair : MaterialsManager::materialsPaths) {
		sceneNode["Materials"].push_back(matPair.second);
	}
#pragma endregion
#pragma region SAVING_MODELS
	for (const auto& modelPair : ModelsManager::modelsPaths) {
		sceneNode["Models"].push_back(modelPair.second);
	}
#pragma endregion
#pragma region SAVING_PREFABS
	for (const auto& prefabPair : PrefabManager::_prefabsPaths) {
		sceneNode["Prefabs"].push_back(prefabPair.second);
	}
#pragma endregion
#pragma region SAVING_GAMEOBJECTS
	Transform* rootT = _rootObject->GetTransform();
	for (i = 0; i < rootT->GetChildCount(); ++i) {
		SaveGameObject(rootT->GetChildAt(i)->GetGameObject(), sceneNode["GameObjects"]);
	}
#pragma endregion

	ofstream file{ path };
	file << sceneNode;
	file.close();
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

GameObject* SceneManager::CreateGameObject(Transform* parent)
{
	GameObject* obj = new GameObject();
	obj->GetTransform()->SetParent(parent == nullptr ? _rootObject->GetTransform() : parent);
	_gameObjectsById[obj->Id()] = obj;
	_componentsById[obj->GetTransform()->GetId()] = obj->GetTransform();
	return obj;
}

GameObject* SceneManager::CreateGameObject(Prefab* prefab, Transform* parent)
{
#pragma region LOADING_PREFAB_TEXTURES
	// TEXTURES
	vector<string> paths;
	for (const auto& path : prefab->_textures) {
		paths.push_back(path.first);
	}

	auto toLoadToUnload = GetResourcesToLoadAndUnload(paths, _texturesIds);

	// Loading
	for (size_t t : toLoadToUnload.first) {
		string path = paths[t];
		Texture2D* temp = nullptr;
		if (prefab->_texturesFormats.find(path) != prefab->_texturesFormats.end()) {
			const auto& formats = prefab->_texturesFormats[path];
			temp = TextureManager::LoadTexture2D(path, formats.second, formats.first, prefab->_textures[path]);
		}
		else {
			temp = TextureManager::LoadTexture2D(path, prefab->_textures[path]);
		}
		if (temp != nullptr) _texturesIds.push_back(temp->GetManagerId());
	}
	// Sorting (First like in prefab file then rest)
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
	// Add rest
	size_t startSortedSize = sortedIds.size();
	for (size_t i = 0; i < _texturesIds.size(); ++i) {
		bool hasId = false;
		for (size_t j = 0; j < startSortedSize; ++j) {
			if (_texturesIds[i] == sortedIds[j]) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			sortedIds.push_back(_texturesIds[i]);
		}
	}
	_texturesIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFAB_SPRITES
	// SPRITES
	paths.clear();
	for (const auto& path : prefab->_sprites) {
		paths.push_back(path.first);
	}

	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _spritesIds);

	// Loading
	for (size_t s : toLoadToUnload.first) {
		string alias = paths[s];
		tuple<string, bool, SpriteData> sData = prefab->_sprites[alias];
		Sprite* temp = nullptr;
		if (get<1>(sData)) {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData), get<2>(sData));
		}
		else {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData));
		}
		if (temp != nullptr) _spritesIds.push_back(temp->GetManagerId());
	}
	// Sorting (First like in prefab file then rest)
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
	// Add rest
	startSortedSize = sortedIds.size();
	for (size_t i = 0; i < _spritesIds.size(); ++i) {
		bool hasId = false;
		for (size_t j = 0; j < startSortedSize; ++j) {
			if (_spritesIds[i] == sortedIds[j]) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			sortedIds.push_back(_spritesIds[i]);
		}
	}
	_spritesIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFAB_FONTS
	// FONTS
	paths.clear();
	for (const auto& path : prefab->_fonts) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _fontsIds);

	// Loading
	for (size_t f : toLoadToUnload.first) {
		Font* temp = FontManager::LoadFont(paths[f]);
		if (temp != nullptr) _fontsIds.push_back(temp->GetManagerId());
	}
	// Sorting (First like in prefab file then rest)
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
	// Add rest
	startSortedSize = sortedIds.size();
	for (size_t i = 0; i < _fontsIds.size(); ++i) {
		bool hasId = false;
		for (size_t j = 0; j < startSortedSize; ++j) {
			if (_fontsIds[i] == sortedIds[j]) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			sortedIds.push_back(_fontsIds[i]);
		}
	}
	_fontsIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFAB_AUDIO
	// AUDIO
	paths.clear();
	for (const auto& path : prefab->_audios) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _audiosIds);

	// Loading
	for (size_t a : toLoadToUnload.first) {
		size_t id = AudioManager::LoadAudio(paths[a]);
		if (id != 0) _audiosIds.push_back(id);
	}
	// Sorting (First like in prefab file then rest)
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
	// Add rest
	startSortedSize = sortedIds.size();
	for (size_t i = 0; i < _audiosIds.size(); ++i) {
		bool hasId = false;
		for (size_t j = 0; j < startSortedSize; ++j) {
			if (_audiosIds[i] == sortedIds[j]) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			sortedIds.push_back(_audiosIds[i]);
		}
	}
	_audiosIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFAB_MATERIALS
	// MATERIALS
	paths.clear();
	for (const auto& path : prefab->_materials) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _materialsIds);

	// Loading
	for (size_t m : toLoadToUnload.first) {
		Material mat = MaterialsManager::LoadMaterial(paths[m]);
		_materialsIds.push_back(mat.GetId());
	}
	// Sorting (First like in prefab file then rest)
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
	// Add rest
	startSortedSize = sortedIds.size();
	for (size_t i = 0; i < _materialsIds.size(); ++i) {
		bool hasId = false;
		for (size_t j = 0; j < startSortedSize; ++j) {
			if (_materialsIds[i] == sortedIds[j]) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			sortedIds.push_back(_materialsIds[i]);
		}
	}
	_materialsIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFAB_MODELS
	// MODELS
	paths.clear();
	for (const auto& path : prefab->_models) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _modelsIds);

	// Loading
	for (size_t m : toLoadToUnload.first) {
		InstatiatingModel model = ModelsManager::LoadModel(paths[m]);
		_modelsIds.push_back(model.GetId());
	}
	// Sorting (First like in prefab file then rest)
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
	// Add rest
	startSortedSize = sortedIds.size();
	for (size_t i = 0; i < _modelsIds.size(); ++i) {
		bool hasId = false;
		for (size_t j = 0; j < startSortedSize; ++j) {
			if (_modelsIds[i] == sortedIds[j]) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			sortedIds.push_back(_materialsIds[i]);
		}
	}
	_modelsIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFAB_PREFABS
	// MODELS
	paths.clear();
	for (const auto& path : prefab->_prefabs) {
		paths.push_back(path);
	}
	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _prefabsIds);

	// Loading
	for (size_t p : toLoadToUnload.first) {
		Prefab* prefab = PrefabManager::LoadPrefab(paths[p]);
		if (prefab == nullptr) {
			SPDLOG_ERROR("Couldn't load prefab {0}", paths[p]);
			continue;
		}
		_prefabsIds.push_back(prefab->GetId());
	}
	// Sorting (First like in prefab file then rest)
	sortedIds.clear();
	for (size_t i = 0; i < paths.size(); ++i) {
		size_t pathH = hash<string>()(paths[i]);
		for (size_t j = 0; j < _prefabsIds.size(); ++j) {
			if (_prefabsIds[j] == pathH) {
				sortedIds.push_back(_prefabsIds[j]);
				break;
			}
		}
	}
	// Add rest
	startSortedSize = sortedIds.size();
	for (size_t i = 0; i < _prefabsIds.size(); ++i) {
		bool hasId = false;
		for (size_t j = 0; j < startSortedSize; ++j) {
			if (_prefabsIds[i] == sortedIds[j]) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			sortedIds.push_back(_prefabsIds[i]);
		}
	}
	_prefabsIds = sortedIds;
#pragma endregion
#pragma region LOADING_PREFAB_GAMEOBJECTS
	// GAME OBJECTS

	// CREATE NEW OBJECTS WITH TRANSFORMS
	map<size_t, GameObject*> objectByComponentId;
	map<size_t, pair<GameObject*, size_t>> idTakenObjects;
	map<size_t, Component*> prefabComponentsById;
	map<size_t, pair<Component*, size_t>> idTakenComponents;

	Action<GameObject*, size_t> CheckTakenID = [&](GameObject* objToReplace, size_t idToTake) -> void {
		if (_gameObjectsById.find(idToTake) != _gameObjectsById.end()) {
			size_t newId = GameObject::GetFreeId();
			idTakenObjects[idToTake] = pair(_gameObjectsById[idToTake], newId);
			_gameObjectsById[newId] = _gameObjectsById[idToTake];
		}
		_gameObjectsById[idToTake] = objToReplace;
	};

	Action<Component*, size_t> CheckComponentTakenID = [&](Component* componentToReplace, size_t idToTake) -> void {
		if (_componentsById.find(idToTake) != _componentsById.end()) {
			size_t newId = Component::GetFreeId();
			idTakenComponents[idToTake] = pair(_componentsById[idToTake], newId);
			_componentsById[newId] = _componentsById[idToTake];
		}
		_componentsById[idToTake] = componentToReplace;
		prefabComponentsById[idToTake] = componentToReplace;
	};

	GameObject* prefabRoot = new GameObject(0);
	CheckTakenID(prefabRoot, 0);

	size_t rootTransformId = prefab->_rootObject["transform"]["id"].as<size_t>();
	CheckComponentTakenID(prefabRoot->GetTransform(), rootTransformId);
	objectByComponentId[rootTransformId] = prefabRoot;

	for (const YAML::Node& gameObjectNode : prefab->_gameObjects) {
		size_t id = gameObjectNode["id"].as<size_t>();
		if (id == 0) {
			SPDLOG_ERROR("Object ID cannot be equal 0");
			continue;
		}
		GameObject* obj = new GameObject(id);
		prefabRoot->GetTransform()->AddChild(obj->GetTransform());
		CheckTakenID(obj, id);

		size_t transformId = gameObjectNode["transform"]["id"].as<size_t>();
		CheckComponentTakenID(obj->GetTransform(), transformId);
		objectByComponentId[transformId] = obj;
	}

	// LOAD ROOT OBJECT AND TRANSFORM VALUES
	map<size_t, YAML::Node> componentsNodes;
	// GameObject
	prefabRoot->SetName(prefab->_rootObject["name"].as<string>());
	prefabRoot->SetIsStatic(prefab->_rootObject["isStatic"].as<bool>());
	prefabRoot->SetActive(prefab->_rootObject["isActive"].as<bool>());

	// Transform
	const YAML::Node& transformNode = prefab->_rootObject["transform"];
	Transform* t = prefabRoot->GetTransform();
	t->SetEnable(transformNode["enabled"].as<bool>());
	t->SetLocalPosition(transformNode["position"].as<glm::vec3>());
	t->SetLocalScale(transformNode["scale"].as<glm::vec3>());
	t->SetLocalRotation(transformNode["rotation"].as<glm::vec3>());

	// Components Node
	componentsNodes[0] = prefab->_rootObject["components"];

	// LOAD GAMEOBJECTS AND TRASFORMS VALUES
	for (const YAML::Node& gameObjectNode : prefab->_gameObjects) {
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
			if (id == 0) {
				SPDLOG_ERROR("Couldn't set rootObject (id: 0) as Child of gameObject");
				continue;
			}
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
			CheckComponentTakenID(comp, id);
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

			if (componentNode["subTypes"]) {
				// Fill Component with SubTypes values
				for (const YAML::Node& subTypeNode : componentNode["subTypes"]) {
					string subType = subTypeNode.as<string>();
					if (!ComponentDeserializer::HasDeserializer(subType)) {
						SPDLOG_ERROR("Couldn't find deserializer for component of subType '{0}'", subType);
						continue;
					}

					ComponentDeserializer::GetDeserializeAction(subType)(comp, componentNode);
				}
			}


			// Fill Component with Type values
			ComponentDeserializer::GetDeserializeAction(type)(comp, componentNode);

			obj->AddComponent(comp);
		}
	}

	// RETURN IDS TO ORIGINAL OBJECTS AND COMPONENTS
	for (const auto& takenIdPair : idTakenObjects) {
		size_t oldId = takenIdPair.first;
		size_t newId = takenIdPair.second.second;

		_gameObjectsById[newId] = _gameObjectsById[oldId];
		_gameObjectsById[newId]->_id = newId;
		_gameObjectsById[oldId] = takenIdPair.second.first;
	}

	for (const auto& takenIdPair : idTakenComponents) {
		size_t oldId = takenIdPair.first;
		size_t newId = takenIdPair.second.second;

		_componentsById[newId] = _componentsById[oldId];
		_componentsById[newId]->_id = newId;
		_componentsById[oldId] = takenIdPair.second.first;

		prefabComponentsById[newId] = _componentsById[newId];
		prefabComponentsById.erase(oldId);

		objectByComponentId[newId] = objectByComponentId[oldId];
		objectByComponentId.erase(oldId);
	}

	// INIT COMPONENTS
	for (const auto& compPair : prefabComponentsById) {
		compPair.second->Init(objectByComponentId[compPair.first], compPair.first);
	}
#pragma endregion


	prefabRoot->GetTransform()->SetParent(parent != nullptr ? parent : _rootObject->GetTransform());
	return prefabRoot;
}

size_t SceneManager::GetCurrentSceneId()
{
	return _currentSceneId;
}

string SceneManager::GetCurrentSceneName()
{
	return _currentSceneName;
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

size_t SceneManager::GetPrefab(size_t loadIdx)
{
	return _prefabsIds[loadIdx];
}

size_t SceneManager::GetTexture2DSaveIdx(size_t texId)
{
	size_t idx = 0;
	for (const auto& texPair : TextureManager::_texturesPaths) {
		if (texPair.first == texId) return idx;
		++idx;
	}
	SPDLOG_WARN("Texture2D '{0}' Not Found", texId);
	return idx;
}

size_t SceneManager::GetSpriteSaveIdx(size_t spriteId)
{
	size_t idx = 0;
	for (const auto& spritePair : SpriteManager::_spriteAliases) {
		if (spritePair.first == spriteId) return idx;
		++idx;
	}
	SPDLOG_WARN("Sprite '{0}' Not Found", spriteId);
	return idx;
}

size_t SceneManager::GetFontSaveIdx(size_t fontId)
{
	size_t idx = 0;
	for (const auto& fontPair : FontManager::_fontsPaths) {
		if (fontPair.first == fontId) return idx;
		++idx;
	}
	SPDLOG_WARN("Font '{0}' Not Found", fontId);
	return idx;
}

size_t SceneManager::GetAudioSaveIdx(size_t audioId)
{
	size_t idx = 0;
	for (const auto& audioPair : AudioManager::_audiosPaths) {
		if (audioPair.first == audioId) return idx;
		++idx;
	}
	SPDLOG_WARN("Audio '{0}' Not Found", audioId);
	return idx;
}

size_t SceneManager::GetMaterialSaveIdx(size_t materialId)
{
	size_t idx = 0;
	for (const auto& matPair : MaterialsManager::materialsPaths) {
		if (matPair.first == materialId) return idx;
		++idx;
	}
	SPDLOG_WARN("Material '{0}' Not Found", materialId);
	return idx;
}

size_t SceneManager::GetModelSaveIdx(size_t modelId)
{
	size_t idx = 0;
	for (const auto& modelPair : ModelsManager::modelsPaths) {
		if (modelPair.first == modelId) return idx;
		++idx;
	}
	SPDLOG_WARN("Model '{0}' Not Found", modelId);
	return idx;
}

size_t SceneManager::GetPrefabSaveIdx(size_t prefabId)
{
	size_t idx = 0;
	for (const auto& prefabPair : PrefabManager::_prefabsPaths) {
		if (prefabPair.first == prefabId) return idx;
		++idx;
	}
	SPDLOG_WARN("Prefab '{0}' Not Found", prefabId);
	return idx;
}

void SceneManager::UnloadCurrent()
{
	_currentSceneId = 0;
	_currentSceneName = "";
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
	for (size_t id : _materialsIds) {
		MaterialsManager::UnloadMaterial(id);
	}
	_materialsIds.clear();
	for (size_t id : _modelsIds) {
		ModelsManager::UnloadModel(id);
	}
	_modelsIds.clear();
	for (size_t id : _prefabsIds) {
		PrefabManager::UnloadPrefab(id);
	}
	_prefabsIds.clear();

	_gameObjectsById.clear();
	_componentsById.clear();
}

void SceneManager::UnloadScene(const std::string& name)
{
	size_t sceneId = hash<string>()(name);
	if (_loadedScenes.find(sceneId) == _loadedScenes.end()) {
		SPDLOG_INFO("Failed to unload scene - Scene '{0}' not found", name);
		return;
	}

	Scene*& scene = _loadedScenes[sceneId];
	if (sceneId == _currentSceneId) {
		UnloadCurrent();
	}
	delete scene;
	_loadedScenes.erase(sceneId);
}

void SceneManager::UnloadAll()
{
	UnloadCurrent();
	for (auto& sceneP : _loadedScenes) {
		delete sceneP.second;
	}
	_loadedScenes.clear();
}