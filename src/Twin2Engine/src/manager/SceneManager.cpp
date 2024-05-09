#include <manager/SceneManager.h>
#include <core/RenderableComponent.h>
#include <core/ComponentDeserializer.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <manager/PrefabManager.h>
#include <manager/ScriptableObjectManager.h>
#include <core/EventHandler.h>
#include <core/ResourceManagement.h>

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
vector<size_t> SceneManager::_scriptableObjectsIds;

map<size_t, Scene*> SceneManager::_loadedScenes;

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
#pragma region LOAD_SCRIPTABLEBJECTS_DATA_FROM_SCENE_FILE
	for (const YAML::Node& scriptableObject : sceneNode["ScriptableObjects"]) {
		scene->AddScriptableObject(scriptableObject["path"].as<string>());
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

	Func<string, const string&> pathGetter;
	Func<bool, size_t> unloader;
	Func<bool, const string&, size_t&> loader;

	Func<vector<size_t>, const vector<size_t>&, const vector<size_t>&> sorter = [](const vector<size_t>& hashedPaths, const vector<size_t>& loadedIds) -> vector<size_t> {
		vector<size_t> sortedIds;
		for (size_t i = 0; i < hashedPaths.size(); ++i) {
			for (size_t j = 0; j < loadedIds.size(); ++j) {
				if (loadedIds[j] == hashedPaths[i]) {
					sortedIds.push_back(loadedIds[j]);
					break;
				}
			}
		}
		return sortedIds;
	};

#pragma region LOADING_TEXTURES
	Func<string, const pair<string, TextureData>&> texturePathGetter = [](const pair<string, TextureData>& texturePair) -> string { return texturePair.first; };
	unloader = [](size_t id) -> bool { TextureManager::UnloadTexture2D(id); return true; };
	loader = [&](const string& path, size_t& id) -> bool {
		Texture2D* temp = nullptr;
		if (sceneToLoad->_texturesFormats.find(path) != sceneToLoad->_texturesFormats.end()) {
			const auto& formats = sceneToLoad->_texturesFormats[path];
			temp = TextureManager::LoadTexture2D(path, formats.second, formats.first, sceneToLoad->_textures[path]);
		}
		else {
			temp = TextureManager::LoadTexture2D(path, sceneToLoad->_textures[path]);
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load texture '{0}'", path);
		return false;
	};
	_texturesIds = LoadResources(texturePathGetter, sceneToLoad->_textures, _texturesIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_SPRITES
	Func<string, const pair<string, tuple<string, bool, SpriteData>>&> spritePathGetter = [](const pair<string, tuple<string, bool, SpriteData>>& spritePair) -> string { return spritePair.first; };
	unloader = [](size_t id) -> bool { SpriteManager::UnloadSprite(id); return true; };
	loader = [&](const string& alias, size_t& id) -> bool { 
		tuple<string, bool, SpriteData> sData = sceneToLoad->_sprites[alias];
		Sprite* temp = nullptr;
		if (get<1>(sData)) {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData), get<2>(sData));
		}
		else {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData));
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't make sprite '{0}'", alias);
		return false;
	};
	_spritesIds = LoadResources(spritePathGetter, sceneToLoad->_sprites, _spritesIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_FONTS
	pathGetter = [](const string& path) -> string { return path; };
	unloader = [](size_t id) -> bool { FontManager::UnloadFont(id); return true; };
	loader = [](const string& path, size_t& id) -> bool { 
		Font* temp = FontManager::LoadFont(path);
		if (temp != nullptr) { 
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load font '{0}'", path);
		return false;
	};
	_fontsIds = LoadResources(pathGetter, sceneToLoad->_fonts, _fontsIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_AUDIO
	pathGetter = [](const string& path) -> string { return path; };
	unloader = [](size_t id) -> bool { AudioManager::UnloadAudio(id); return true; };
	loader = [](const string& path, size_t& id) -> bool { 
		id = AudioManager::LoadAudio(path); 
		if (id != 0) return true; 
		SPDLOG_ERROR("Couldn't load audio '{0}'", path);
		return false;
	};
	_audiosIds = LoadResources(pathGetter, sceneToLoad->_audios, _audiosIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_MATERIALS
	pathGetter = [](const string& path) -> string { return path; };
	unloader = [](size_t id) -> bool { MaterialsManager::UnloadMaterial(id); return true; };
	loader = [](const string& path, size_t& id) -> bool { id = MaterialsManager::LoadMaterial(path).GetId(); return true; };
	_materialsIds = LoadResources(pathGetter, sceneToLoad->_materials, _materialsIds, unloader, loader, sorter);

#pragma endregion
#pragma region LOADING_MODELS
	pathGetter = [](const string& path) -> string { return path; };
	unloader = [](size_t id) -> bool { ModelsManager::UnloadModel(id); return true; };
	loader = [](const string& path, size_t& id) -> bool { id = ModelsManager::LoadModel(path).GetId(); return true; };
	_modelsIds = LoadResources(pathGetter, sceneToLoad->_models, _modelsIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_PREFABS
	pathGetter = [](const string& path) -> string { return path; };
	unloader = [](size_t id) -> bool { PrefabManager::UnloadPrefab(id); return true; };
	loader = [](const string& path, size_t& id) -> bool { 
		Prefab* prefab = PrefabManager::LoadPrefab(path);
		if (prefab != nullptr) {
			id = prefab->GetId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load prefab {0}", path);
		return false;
	};
	_prefabsIds = LoadResources(pathGetter, sceneToLoad->_prefabs, _prefabsIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_SCRIPTABLE_OBJECTS
	ScriptableObjectManager::SceneDeserializationBegin();
	for (unsigned int i = 0; i < sceneToLoad->_scriptableObjects.size(); i++)
	{
		size_t id = ScriptableObjectManager::SceneDeserialize(0, sceneToLoad->_scriptableObjects[i]);
		if (std::find_if(_scriptableObjectsIds.cbegin(), _scriptableObjectsIds.cend(), [id](const size_t& soId) { return soId == id; }) == _scriptableObjectsIds.end())
		{
			_scriptableObjectsIds.push_back(id);
		}
		//_scriptableObjectsIds.push_back(id);
	}
	//pathGetter = [](const string& path) -> string { return path; };
	//unloader = [](size_t id) -> bool { return true; };
	//loader = [](const string& path, size_t& id) -> bool {
	//	ScriptableObject* scriptableObject = ScriptableObjectManager::Load(path);
	//	if (scriptableObject != nullptr) {
	//		id = scriptableObject->GetId();
	//		return true;
	//	}
	//	SPDLOG_ERROR("Couldn't load scriptableObject {0}", path);
	//	return false;
	//	};
	//_scriptableObjectsIds = LoadResources(pathGetter, sceneToLoad->_scriptableObjects, _scriptableObjectsIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_GAMEOBJECTS
	// GAME OBJECTS

	// DESTROY OLD OBJECTS
	if (_rootObject != nullptr) {
		_gameObjectsById.clear();
		_componentsById.clear();
		DestroyGameObject(_rootObject);
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
			size_t id = componentNode["id"].as<size_t>();
			_componentsById[id] = ComponentDeserializer::GetComponentFunc(type)();
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

	ScriptableObjectManager::SceneDeserializationEnd();

	_currentSceneName = name;
	_currentSceneId = sceneId;
}

void SceneManager::SaveScene(const string& path) {
	YAML::Node sceneNode;

#pragma region SAVING_TEXTURES
	sceneNode["Textures"] = TextureManager::Serialize();
#pragma endregion
#pragma region SAVING_SPRITES
	sceneNode["Sprites"] = SpriteManager::Serialize();
	for (YAML::Node spriteNode : sceneNode["Sprites"]) {
		spriteNode["texture"] = GetTexture2DSaveIdx(spriteNode["texture"].as<size_t>());
	}
#pragma endregion
#pragma region SAVING_FONTS
	sceneNode["Fonts"] = FontManager::Serialize();
#pragma endregion
#pragma region SAVING_AUDIOS
	sceneNode["Audio"] = AudioManager::Serialize();
#pragma endregion
#pragma region SAVING_MATERIALS
	sceneNode["Materials"] = MaterialsManager::Serialize();
#pragma endregion
#pragma region SAVING_MODELS
	sceneNode["Models"] = ModelsManager::Serialize();
#pragma endregion
#pragma region SAVING_PREFABS
	sceneNode["Prefabs"] = PrefabManager::Serialize();
#pragma endregion
#pragma region SAVING_SCRIPTABLE_OBJECTS
	sceneNode["ScriptableObjects"];
	ScriptableObjectManager::SceneSerializationBegin();
#pragma endregion
#pragma region SAVING_GAMEOBJECTS
	Transform* rootT = _rootObject->GetTransform();
	for (size_t i = 0; i < rootT->GetChildCount(); ++i) {
		SaveGameObject(rootT->GetChildAt(i)->GetGameObject(), sceneNode["GameObjects"]);
	}
#pragma endregion

	sceneNode["ScriptableObjects"] = ScriptableObjectManager::Serialize();
	ScriptableObjectManager::SceneSerializationEnd();

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

void SceneManager::DestroyGameObject(GameObject* obj)
{
	Transform* trans = obj->GetTransform();
	trans->SetParent(nullptr);
	while (trans->GetChildCount() > 0) {
		Transform* child = trans->GetChildAt(0);
		trans->RemoveChild(child);

		GameObject* childObj = child->GetGameObject();
		if (_gameObjectsById.size() > 0) _gameObjectsById.erase(childObj->Id());
		DestroyGameObject(childObj);
	}
	delete obj;
}

GameObject* SceneManager::CreateGameObject(Transform* parent)
{
	GameObject* obj = new GameObject();
	if (_rootObject == nullptr) _rootObject = new GameObject();
	obj->GetTransform()->SetParent(parent != nullptr ? parent : _rootObject->GetTransform());
	_gameObjectsById[obj->Id()] = obj;
	_componentsById[obj->GetTransform()->GetId()] = obj->GetTransform();
	return obj;
}

GameObject* SceneManager::CreateGameObject(Prefab* prefab, Transform* parent)
{
	Func<string, const string&> pathGetter;
	Func<bool, size_t> unloader = [](size_t id) -> bool { return false; };
	Func<bool, const string&, size_t&> loader;

	Func<vector<size_t>, const vector<size_t>&, const vector<size_t>&> sorter = [](const vector<size_t>& hashedPaths, const vector<size_t>& loadedIds) -> vector<size_t> {
		// Sorting (First like in prefab file then rest)
		vector<size_t> sortedIds;
		for (size_t i = 0; i < hashedPaths.size(); ++i) {
			for (size_t j = 0; j < loadedIds.size(); ++j) {
				if (loadedIds[j] == hashedPaths[i]) {
					sortedIds.push_back(loadedIds[j]);
					break;
				}
			}
		}
		// Add rest
		size_t startSortedSize = sortedIds.size();
		for (size_t i = 0; i < loadedIds.size(); ++i) {
			bool hasId = false;
			for (size_t j = 0; j < startSortedSize; ++j) {
				if (loadedIds[i] == sortedIds[j]) {
					hasId = true;
					break;
				}
			}
			if (!hasId) {
				sortedIds.push_back(loadedIds[i]);
			}
		}
		return sortedIds;
	};

#pragma region LOADING_PREFAB_TEXTURES
	Func<string, const pair<string, TextureData>&> texturePathGetter = [](const pair<string, TextureData>& texturePair) -> string { return texturePair.first; };
	loader = [&](const string& path, size_t& id) -> bool {
		Texture2D* temp = nullptr;
		if (prefab->_texturesFormats.find(path) != prefab->_texturesFormats.end()) {
			const auto& formats = prefab->_texturesFormats[path];
			temp = TextureManager::LoadTexture2D(path, formats.second, formats.first, prefab->_textures[path]);
		}
		else {
			temp = TextureManager::LoadTexture2D(path, prefab->_textures[path]);
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load texture '{0}'", path);
		return false;
	};
	_texturesIds = LoadResources(texturePathGetter, prefab->_textures, _texturesIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_PREFAB_SPRITES
	Func<string, const pair<string, tuple<string, bool, SpriteData>>&> spritePathGetter = [](const pair<string, tuple<string, bool, SpriteData>>& spritePair) -> string { return spritePair.first; };
	loader = [&](const string& alias, size_t& id) -> bool {
		tuple<string, bool, SpriteData> sData = prefab->_sprites[alias];
		Sprite* temp = nullptr;
		if (get<1>(sData)) {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData), get<2>(sData));
		}
		else {
			temp = SpriteManager::MakeSprite(alias, get<0>(sData));
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't make sprite '{0}'", alias);
		return false;
	};
	_spritesIds = LoadResources(spritePathGetter, prefab->_sprites, _spritesIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_PREFAB_FONTS
	pathGetter = [](const string& path) -> string { return path; };
	loader = [](const string& path, size_t& id) -> bool {
		Font* temp = FontManager::LoadFont(path);
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load font '{0}'", path);
		return false;
		};
	_fontsIds = LoadResources(pathGetter, prefab->_fonts, _fontsIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_PREFAB_AUDIO
	pathGetter = [](const string& path) -> string { return path; };
	loader = [](const string& path, size_t& id) -> bool {
		id = AudioManager::LoadAudio(path);
		if (id != 0) return true;
		SPDLOG_ERROR("Couldn't load audio '{0}'", path);
		return false;
		};
	_audiosIds = LoadResources(pathGetter, prefab->_audios, _audiosIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_PREFAB_MATERIALS
	pathGetter = [](const string& path) -> string { return path; };
	loader = [](const string& path, size_t& id) -> bool { id = MaterialsManager::GetMaterial(path).GetId(); return true; };
	_materialsIds = LoadResources(pathGetter, prefab->_materials, _materialsIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_PREFAB_MODELS
	pathGetter = [](const string& path) -> string { return path; };
	loader = [](const string& path, size_t& id) -> bool { id = ModelsManager::LoadModel(path).GetId(); return true; };
	_modelsIds = LoadResources(pathGetter, prefab->_models, _modelsIds, unloader, loader, sorter);
#pragma endregion
#pragma region LOADING_PREFAB_PREFABS
	pathGetter = [](const string& path) -> string { return path; };
	loader = [](const string& path, size_t& id) -> bool {
		Prefab* prefab = PrefabManager::LoadPrefab(path);
		if (prefab != nullptr) {
			id = prefab->GetId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load prefab {0}", path);
		return false;
		};
	_prefabsIds = LoadResources(pathGetter, prefab->_prefabs, _prefabsIds, unloader, loader, sorter);
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

	if (_rootObject == nullptr) _rootObject = new GameObject();
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
	_gameObjectsById.clear();
	_componentsById.clear();
	DestroyGameObject(_rootObject);

	Action<vector<size_t>&, const Action<size_t>&> unloader = [](vector<size_t>& ids, const Action<size_t>& unload) -> void {
		for (size_t id : ids) {
			unload(id);
		}
		ids.clear();
	};

	unloader(_texturesIds, [](size_t id) -> void { TextureManager::UnloadTexture2D(id); });
	unloader(_spritesIds, [](size_t id) -> void { SpriteManager::UnloadSprite(id); });
	unloader(_fontsIds, [](size_t id) -> void { FontManager::UnloadFont(id); });
	unloader(_audiosIds, [](size_t id) -> void { AudioManager::UnloadAudio(id); });
	unloader(_materialsIds, [](size_t id) -> void { MaterialsManager::UnloadMaterial(id); });
	unloader(_modelsIds, [](size_t id) -> void { ModelsManager::UnloadModel(id); });
	unloader(_prefabsIds, [](size_t id) -> void { PrefabManager::UnloadPrefab(id); });
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