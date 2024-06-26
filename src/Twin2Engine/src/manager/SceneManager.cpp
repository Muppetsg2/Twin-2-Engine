#include <manager/SceneManager.h>
#include <core/RenderableComponent.h>
#include <core/ComponentsMap.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <graphic/manager/GIFManager.h>
#include <manager/PrefabManager.h>
#include <tools/EventHandler.h>
#include <manager/ScriptableObjectManager.h>
#include <core/ResourceManagement.h>
#include <regex>

#include <graphic/manager/MeshRenderingManager.h>

using namespace Twin2Engine::Manager;
using namespace std;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Tools;
using namespace Twin2Engine::Graphic;

size_t SceneManager::_currentSceneId;
std::string SceneManager::_currentSceneName;
GameObject* SceneManager::_rootObject = nullptr;

map<size_t, GameObject*> SceneManager::_gameObjectsById;
map<size_t, Component*> SceneManager::_componentsById;

map<size_t, size_t> SceneManager::_texturesIds;
map<size_t, size_t> SceneManager::_spritesIds;
map<size_t, size_t> SceneManager::_fontsIds;
map<size_t, size_t> SceneManager::_gifsIds;
map<size_t, size_t> SceneManager::_audiosIds;
map<size_t, size_t> SceneManager::_materialsIds;
map<size_t, size_t> SceneManager::_modelsIds;
map<size_t, size_t> SceneManager::_prefabsIds;
vector<size_t> SceneManager::_scriptableObjectsIds;

map<size_t, Scene*> SceneManager::_loadedScenes;
map<size_t, string> SceneManager::_loadedScenesNames;
map<size_t, string> SceneManager::_loadedScenesPaths;

#if _DEBUG
ImGuiID SceneManager::_selected = 0;
bool SceneManager::_inspectorOpened = true;
const std::string SceneManager::_payloadType = "SceneHierarchyObject";
#endif

void SceneManager::SaveGameObject(const GameObject* obj, YAML::Node gameObjects)
{
	gameObjects.push_back(obj->Serialize());

	Transform* objT = obj->GetTransform();
	for (size_t i = 0; i < objT->GetChildCount(); ++i) {
		SaveGameObject(objT->GetChildAt(i)->GetGameObject(), gameObjects);
	}
}

#if _DEBUG
void SceneManager::DrawGameObjectEditor(const Core::GameObject* obj)
{
	size_t clicked_elem = 0;
	Transform* objT = obj->GetTransform();

	for (size_t i = 0; i < objT->GetChildCount(); ++i) {

		//ImGuiDragDropFlags drop_target_flags = ImGuiDragDropFlags_AcceptBeforeDelivery;
		ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		const string name = objT->GetChildAt(i)->GetName().append("##").append(objT->GetName()).append(std::to_string(i));
		const size_t id = objT->GetChildAt(i)->GetGameObject()->Id();
		const bool is_selected = id == _selected;

		if (is_selected) {
			node_flag |= ImGuiTreeNodeFlags_Selected;

			if (ImGui::Begin("Inspector", &_inspectorOpened)) {
				objT->GetChildAt(i)->GetGameObject()->DrawEditor();
			}
			ImGui::End();

			if (!_inspectorOpened) {
				_selected = 0;
			}
		}

		if (objT->GetChildAt(i) == nullptr)
			return;

		if (objT->GetChildAt(i)->GetChildCount() == 0) {
			node_flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx(name.c_str(), node_flag);
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
				clicked_elem = id;
			}

			if (ImGui::BeginDragDropTarget()) {

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(_payloadType.c_str()))
				{
					size_t payload_n = *(const size_t*)payload->Data;
					Transform* t = SceneManager::GetGameObjectWithId(payload_n)->GetTransform();
					if (objT->GetChildAt(i) != t) {
						_selected = objT->GetChildAt(i)->GetGameObject()->Id();
						t->SetParent(objT->GetChildAt(i));
					}
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource()) {
				size_t n = objT->GetChildAt(i)->GetGameObject()->Id();
				ImGui::SetDragDropPayload(_payloadType.c_str(), &n, sizeof(size_t), ImGuiCond_Once);
				ImGui::Text(objT->GetChildAt(i)->GetName().c_str());
				ImGui::EndDragDropSource();
			}
		}
		else {
			bool node_open = ImGui::TreeNodeEx(name.c_str(), node_flag);

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
				clicked_elem = id;
			}

			if (ImGui::BeginDragDropTarget()) {

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(_payloadType.c_str()))
				{
					size_t payload_n = *(const size_t*)payload->Data;
					Transform* t = SceneManager::GetGameObjectWithId(payload_n)->GetTransform();
					if (i < objT->GetChildCount() && objT->GetChildAt(i) != t) {
						t->SetParent(objT->GetChildAt(i));
						_selected = objT->GetChildAt(i)->GetGameObject()->Id();
					}
					else if (i >= objT->GetChildCount()) {
						_selected = 0;
					}
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource()) {
				size_t n = objT->GetChildAt(i)->GetGameObject()->Id();
				ImGui::SetDragDropPayload(_payloadType.c_str(), &n, sizeof(size_t), ImGuiCond_Once);
				ImGui::Text(objT->GetChildAt(i)->GetName().c_str());
				ImGui::EndDragDropSource();
			}

			if (node_open) {
				DrawGameObjectEditor(objT->GetChildAt(i)->GetGameObject());
				ImGui::TreePop();
			}
		}
	}

	if (clicked_elem != 0) {
		_selected = clicked_elem;
		_inspectorOpened = true;
	}
}
#endif

GameObject* SceneManager::FindObjectBy(GameObject* obj, const Func<bool, const GameObject*>& predicate)
{
	if (obj == nullptr) return nullptr;

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

EventHandler<string> SceneManager::_onSceneLoaded;

bool SceneManager::_loadScene = false;
string SceneManager::_sceneToLoadName = "";
size_t SceneManager::_sceneToLoadId = 0;

void SceneManager::LoadScene() {
	if (!_loadScene) return;
	_loadScene = false;

	Scene* sceneToLoad = _loadedScenes[_sceneToLoadId];

	Func<string, const pair<size_t, string>&> pathGetter = [](const pair<size_t, string>& pair) -> string { return pair.second; };
	Func<size_t, const pair<size_t, string>&> idGetter = [](const pair<size_t, string>& pair) -> size_t { return pair.first; };
	Func<string, const pair<size_t, string>&> dataGetter = [](const pair<size_t, string>& pair) -> string { return pair.second; };
	Func<bool, size_t> unloader;
	Func<bool, const string&, size_t&> loader;

	// DESTROY OLD OBJECTS
	if (_rootObject != nullptr) {
		_gameObjectsById.clear();
		_componentsById.clear();
		DestroyObject(_rootObject);
	}

#pragma region LOADING_TEXTURES
	Func<string, const pair<size_t, pair<string, TextureData>>&> texturePathGetter = [](const pair<size_t, pair<string, TextureData>>& texturePair) -> string { return texturePair.second.first; };
	Func<size_t, const pair<size_t, pair<string, TextureData>>&> textureIdGetter = [](const pair<size_t, pair<string, TextureData>>& texturePair) -> size_t { return texturePair.first; };
	Func<pair<string, TextureData>, const pair<size_t, pair<string, TextureData>>&> textureDataGetter = [](const pair<size_t, pair<string, TextureData>>& texturePair) -> pair<string, TextureData> { return texturePair.second; };
	unloader = [](size_t id) -> bool { TextureManager::UnloadTexture2D(id); return true; };
	Func<bool, const pair<string, TextureData>&, size_t&> textureLoader = [&](const pair<string, TextureData>& textureLoadData, size_t& id) -> bool {
		Texture2D* temp = nullptr;
		if (sceneToLoad->_texturesFormats.contains(textureLoadData.first)) {
			const auto& formats = sceneToLoad->_texturesFormats[textureLoadData.first];
			temp = TextureManager::LoadTexture2D(textureLoadData.first, formats.second, formats.first, textureLoadData.second);
		}
		else {
			temp = TextureManager::LoadTexture2D(textureLoadData.first, textureLoadData.second);
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load texture '{0}'", textureLoadData.first);
		return false;
		};
	_texturesIds = LoadResources(texturePathGetter, textureIdGetter, textureDataGetter, sceneToLoad->_textures, _texturesIds, unloader, textureLoader);
#pragma endregion
#pragma region LOADING_SPRITES
	Func<string, const pair<size_t, tuple<string, size_t, bool, SpriteData>>&> spritePathGetter = [](const pair<size_t, tuple<string, size_t, bool, SpriteData>>& spritePair) -> string { return get<0>(spritePair.second); };
	Func<size_t, const pair<size_t, tuple<string, size_t, bool, SpriteData>>&> spriteIdGetter = [](const pair<size_t, tuple<string, size_t, bool, SpriteData>>& spritePair) -> size_t { return spritePair.first; };
	Func<tuple<string, size_t, bool, SpriteData>, const pair<size_t, tuple<string, size_t, bool, SpriteData>>&> spriteDataGetter = [](const pair<size_t, tuple<string, size_t, bool, SpriteData>>& spritePair) -> tuple<string, size_t, bool, SpriteData> { return spritePair.second; };
	unloader = [](size_t id) -> bool { SpriteManager::UnloadSprite(id); return true; };
	Func<bool, const tuple<string, size_t, bool, SpriteData>&, size_t&> spriteLoader = [&](const tuple<string, size_t, bool, SpriteData>& spriteLoadData, size_t& id) -> bool {
		Sprite* temp = nullptr;
		if (get<2>(spriteLoadData)) {
			temp = SpriteManager::MakeSprite(get<0>(spriteLoadData), _texturesIds[get<1>(spriteLoadData)], get<3>(spriteLoadData));
		}
		else {
			temp = SpriteManager::MakeSprite(get<0>(spriteLoadData), _texturesIds[get<1>(spriteLoadData)]);
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't make sprite '{0}'", get<0>(spriteLoadData));
		return false;
		};
	_spritesIds = LoadResources(spritePathGetter, spriteIdGetter, spriteDataGetter, sceneToLoad->_sprites, _spritesIds, unloader, spriteLoader);
#pragma endregion
#pragma region LOADING_FONTS
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
	_fontsIds = LoadResources(pathGetter, idGetter, dataGetter, sceneToLoad->_fonts, _fontsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_GIFS
	unloader = [](size_t id) -> bool { GIFManager::Unload(id); return true; };
	loader = [](const string& path, size_t& id) -> bool {
		GIF* temp = GIFManager::Load(path);
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load gif '{0}'", path);
		return false;
	};
	_gifsIds = LoadResources(pathGetter, idGetter, dataGetter, sceneToLoad->_gifs, _gifsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_AUDIO
	unloader = [](size_t id) -> bool { AudioManager::UnloadAudio(id); return true; };
	loader = [](const string& path, size_t& id) -> bool {
		id = AudioManager::LoadAudio(path);
		if (id != 0) return true;
		SPDLOG_ERROR("Couldn't load audio '{0}'", path);
		return false;
		};
	_audiosIds = LoadResources(pathGetter, idGetter, dataGetter, sceneToLoad->_audios, _audiosIds, unloader, loader);
#pragma endregion
#pragma region LOADING_MATERIALS
	unloader = [](size_t id) -> bool { MaterialsManager::UnloadMaterial(id); return true; };
	loader = [](const string& path, size_t& id) -> bool { id = MaterialsManager::LoadMaterial(path)->GetId(); return true; };
	_materialsIds = LoadResources(pathGetter, idGetter, dataGetter, sceneToLoad->_materials, _materialsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_MODELS
	unloader = [](size_t id) -> bool { ModelsManager::UnloadModel(id); return true; };
	loader = [](const string& path, size_t& id) -> bool { id = ModelsManager::LoadModel(path).GetId(); return true; };
	_modelsIds = LoadResources(pathGetter, idGetter, dataGetter, sceneToLoad->_models, _modelsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_PREFABS
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
	_prefabsIds = LoadResources(pathGetter, idGetter, dataGetter, sceneToLoad->_prefabs, _prefabsIds, unloader, loader);
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

	// CREATE NEW OBJECTS WITH TRANSFORMS
	_rootObject = new GameObject(0);
	_rootObject->SetName("##ROOT##");
	_gameObjectsById[0] = _rootObject;
	_componentsById[0] = _rootObject->GetTransform();
	_rootObject->GetTransform()->Init(_rootObject, 0);

	for (const YAML::Node& gameObjectNode : sceneToLoad->_gameObjects) {
		size_t id = gameObjectNode["id"].as<size_t>();
		if (id == 0) {
			SPDLOG_ERROR("Object ID cannot be equal 0. Skipping...");
			continue;
		}
		GameObject* obj = new GameObject(id);
		_rootObject->GetTransform()->AddChild(obj->GetTransform());
		_gameObjectsById[id] = obj;

		size_t transformId = gameObjectNode["transform"]["id"].as<size_t>();
		Component* comp = obj->GetTransform();
		_componentsById[transformId] = comp;
		comp->Init(obj, transformId);
	}

	// LOAD GAMEOBJECTS AND TRASFORMS VALUES
	map<size_t, YAML::Node> componentsNodes;
	for (const YAML::Node& gameObjectNode : sceneToLoad->_gameObjects) {
		// GameObject
		size_t objId = gameObjectNode["id"].as<size_t>();
		GameObject* obj = _gameObjectsById[objId];
		if (!obj->Deserialize(gameObjectNode)) {
			SPDLOG_ERROR("Scene '{0}' GameObject {1} data is corupted", _sceneToLoadName, objId);
		}

		// Transform
		Transform* t = obj->GetTransform();
		if (!t->Deserialize(gameObjectNode["transform"])) {
			SPDLOG_ERROR("Scene '{0}' Transform of GameObject {0} data is corupted", _sceneToLoadName, objId);
		}

		// Components Node
		componentsNodes[objId] = gameObjectNode["components"];

		// Set Children
		for (const YAML::Node& childNode : gameObjectNode["children"]) {
			size_t id = childNode.as<size_t>();
			if (id == 0) {
				SPDLOG_ERROR("Couldn't set rootObject (id: 0) as Child of gameObject. Skipping...");
				continue;
			}
			if (!_gameObjectsById.contains(id)) {
				SPDLOG_ERROR("Couldn't find gameObject with id: {0}, provided in children list of gameObject {1}. Skipping...", id, objId);
				continue;
			}
			t->AddChild(_gameObjectsById[id]->GetTransform());
		}
	}

	// UPDATE ACTIVE FLAG
	for (auto& item : _gameObjectsById) {
		if (!item.second->GetActiveSelf())
			item.second->UpdateActiveInChildren();
	}

	// CREATE AND ADD COMPONENTS TO OBJECTS
	for (const auto& compPair : componentsNodes) {
		GameObject* obj = _gameObjectsById[compPair.first];
		for (const YAML::Node& componentNode : compPair.second) {
			string type = componentNode["type"].as<string>();
			if (!ComponentsMap::HasComponent(type)) {
				SPDLOG_ERROR("Couldn't find create function for component of type '{0}'. Skipping...", type);
				continue;
			}

			// Create Component of type
			size_t id = componentNode["id"].as<size_t>();
			Component* comp = ComponentsMap::CreateComponent(type);
			_componentsById[id] = comp;

			// Add Component to object
			obj->AddComponentNoInit(comp);
			comp->Init(obj, id);
		}
	}

	// LOAD COMPONENTS VALUES
	for (const auto& compPair : componentsNodes) {
		GameObject* obj = _gameObjectsById[compPair.first];
		for (const YAML::Node& componentNode : compPair.second) {
			string type = componentNode["type"].as<string>();
			if (!ComponentsMap::HasComponent(type)) {
				SPDLOG_ERROR("Couldn't find create function for component of type '{0}'. Skipping...", type);
				continue;
			}

			size_t compId = componentNode["id"].as<size_t>();
			Component* comp = _componentsById[compId];

			// Fill Component with values
			if (!comp->Deserialize(componentNode)) {
				SPDLOG_ERROR("Scene '{0}' '{1}' Component {2} of GameObject {3} data is corupted", _sceneToLoadName, type, compId, compPair.first);
			}
		}
	}

	// INIT COMPONENTS
	auto componentsById_copy(_componentsById);
	for (const auto& compPair : componentsById_copy) {
		compPair.second->Initialize();
	}
#pragma endregion

	// ENABLE COMPONENTS
	for (const auto& compPair : componentsById_copy) {
		if (compPair.second->_enabled)
		{
			compPair.second->OnEnable();
		}
		else {
			compPair.second->OnDisable();
		}
	}

	ScriptableObjectManager::SceneDeserializationEnd();

	_currentSceneName = _sceneToLoadName;
	_currentSceneId = _sceneToLoadId;

	_onSceneLoaded(_sceneToLoadName);
}

queue<GameObject*> SceneManager::_objectsToDestroy;

void SceneManager::DestroyObject(GameObject* obj) {
	Transform* trans = obj->GetTransform();
	trans->SetParent(nullptr);
	while (trans->GetChildCount() > 0) {
		Transform* child = trans->GetChildAt(0);
		trans->RemoveChild(child);

		DestroyObject(child->GetGameObject());
	}

	if (_gameObjectsById.size() > 0) _gameObjectsById.erase(obj->Id());

	for (auto& comp : obj->GetComponents<Component>()) {
		if (_componentsById.size() > 0) _componentsById.erase(comp->GetId());
	}

	delete obj;
}

void SceneManager::DestroyObjects() {
	if (_objectsToDestroy.size() == 0) return;

	while (_objectsToDestroy.size() > 0) {
		DestroyObject(_objectsToDestroy.front());
		_objectsToDestroy.pop();
	}
}

void SceneManager::AddComponentWithId(Component* comp)
{
	_componentsById[comp->GetId()] = comp;
}

void SceneManager::RemoveComponentWithId(Component* comp)
{
	map<size_t, Component*>::const_iterator comp_iter = _componentsById.find(comp->GetId());
	if (comp_iter != _componentsById.end()) {
		if ((*comp_iter).second == comp) {
			_componentsById.erase((*comp_iter).first);
		}
	}
}

void SceneManager::AddScene(const string& name, Scene* scene)
{
	size_t id = hash<string>()(name);
	if (_loadedScenes.contains(id)) {
		SPDLOG_WARN("Replacing a Scene Named '{0}'", name);
	}
	_loadedScenes[id] = scene;
	_loadedScenesNames[id] = name;
}

void SceneManager::AddScene(const string& name, const string& path)
{
	if (filesystem::exists(path)) {
		Scene* scene = new Scene();
		scene->Deserialize(YAML::LoadFile(path));
		size_t id = hash<string>()(name);
		if (_loadedScenesPaths.contains(id)) {
			SPDLOG_WARN("Replacing a Scene Named '{0}'", name);
		}
		_loadedScenesPaths[id] = path;
		AddScene(name, scene);
	}
	else {
		SPDLOG_ERROR("Scene file '{0}' not found!", path);
	}
}

void SceneManager::LoadScene(const string& name)
{
	hash<string> hasher;
	size_t sceneId = hasher(name);
	if (!_loadedScenes.contains(sceneId)) {
		SPDLOG_WARN("No scene named '{0}' found", name);
		return;
	}

	_loadScene = true;
	_sceneToLoadId = sceneId;
	_sceneToLoadName = name;
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
#pragma region SAVING_GIFS
	sceneNode["GIFS"] = GIFManager::Serialize();
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

	SPDLOG_INFO("Scene Saved in '{0}'", path.c_str());
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

void SceneManager::Update()
{
	DestroyObjects();
	LoadScene();
}

EventHandler<string>& SceneManager::GetOnSceneLoaded()
{
	return _onSceneLoaded;
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
	_objectsToDestroy.push(obj);
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
	Func<string, const pair<size_t, string>&> pathGetter = [](const pair<size_t, string>& pair) -> string { return pair.second; };
	Func<size_t, const pair<size_t, string>&> idGetter = [](const pair<size_t, string>& pair) -> size_t { return pair.first; };
	Func<string, const pair<size_t, string>&> dataGetter = [](const pair<size_t, string>& pair) -> string { return pair.second; };
	Func<bool, size_t> unloader = [](size_t id) -> bool { return false; };
	Func<bool, const string&, size_t&> loader;

#pragma region LOADING_PREFAB_TEXTURES
	Func<string, const pair<size_t, pair<string, TextureData>>&> texturePathGetter = [](const pair<size_t, pair<string, TextureData>>& texturePair) -> string { return texturePair.second.first; };
	Func<size_t, const pair<size_t, pair<string, TextureData>>&> textureIdGetter = [](const pair<size_t, pair<string, TextureData>>& texturePair) -> size_t { return texturePair.first; };
	Func<pair<string, TextureData>, const pair<size_t, pair<string, TextureData>>&> textureDataGetter = [](const pair<size_t, pair<string, TextureData>>& texturePair) -> pair<string, TextureData> { return texturePair.second; };
	Func<bool, const pair<string, TextureData>&, size_t&> textureLoader = [&](const pair<string, TextureData>& textureLoadData, size_t& id) -> bool {
		Texture2D* temp = nullptr;
		if (prefab->_texturesFormats.contains(textureLoadData.first)) {
			const auto& formats = prefab->_texturesFormats[textureLoadData.first];
			temp = TextureManager::LoadTexture2D(textureLoadData.first, formats.second, formats.first, textureLoadData.second);
		}
		else {
			temp = TextureManager::LoadTexture2D(textureLoadData.first, textureLoadData.second);
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load texture '{0}'", textureLoadData.first);
		return false;
	};
	_texturesIds = LoadResources(texturePathGetter, textureIdGetter, textureDataGetter, prefab->_textures, _texturesIds, unloader, textureLoader);
#pragma endregion
#pragma region LOADING_PREFAB_SPRITES
	Func<string, const pair<size_t, tuple<string, size_t, bool, SpriteData>>&> spritePathGetter = [](const pair<size_t, tuple<string, size_t, bool, SpriteData>>& spritePair) -> string { return get<0>(spritePair.second); };
	Func<size_t, const pair<size_t, tuple<string, size_t, bool, SpriteData>>&> spriteIdGetter = [](const pair<size_t, tuple<string, size_t, bool, SpriteData>>& spritePair) -> size_t { return spritePair.first; };
	Func<tuple<string, size_t, bool, SpriteData>, const pair<size_t, tuple<string, size_t, bool, SpriteData>>&> spriteDataGetter = [](const pair<size_t, tuple<string, size_t, bool, SpriteData>>& spritePair) -> tuple<string, size_t, bool, SpriteData> { return spritePair.second; };
	Func<bool, const tuple<string, size_t, bool, SpriteData>&, size_t&> spriteLoader = [&](const tuple<string, size_t, bool, SpriteData>& spriteLoadData, size_t& id) -> bool {
		Sprite* temp = nullptr;
		if (get<2>(spriteLoadData)) {
			temp = SpriteManager::MakeSprite(get<0>(spriteLoadData), _texturesIds[get<1>(spriteLoadData)], get<3>(spriteLoadData));
		}
		else {
			temp = SpriteManager::MakeSprite(get<0>(spriteLoadData), _texturesIds[get<1>(spriteLoadData)]);
		}
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't make sprite '{0}'", get<0>(spriteLoadData));
		return false;
	};
	_spritesIds = LoadResources(spritePathGetter, spriteIdGetter, spriteDataGetter, prefab->_sprites, _spritesIds, unloader, spriteLoader);
#pragma endregion
#pragma region LOADING_PREFAB_FONTS
	loader = [](const string& path, size_t& id) -> bool {
		Font* temp = FontManager::LoadFont(path);
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load font '{0}'", path);
		return false;
	};
	_fontsIds = LoadResources(pathGetter, idGetter, dataGetter, prefab->_fonts, _fontsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_PREFAB_GIFS
	loader = [](const string& path, size_t& id) -> bool {
		GIF* temp = GIFManager::Load(path);
		if (temp != nullptr) {
			id = temp->GetManagerId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load gif '{0}'", path);
		return false;
	};
	_gifsIds = LoadResources(pathGetter, idGetter, dataGetter, prefab->_gifs, _gifsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_PREFAB_AUDIO
	loader = [](const string& path, size_t& id) -> bool {
		id = AudioManager::LoadAudio(path);
		if (id != 0) return true;
		SPDLOG_ERROR("Couldn't load audio '{0}'", path);
		return false;
	};
	_audiosIds = LoadResources(pathGetter, idGetter, dataGetter, prefab->_audios, _audiosIds, unloader, loader);
#pragma endregion
#pragma region LOADING_PREFAB_MATERIALS
	loader = [](const string& path, size_t& id) -> bool { id = MaterialsManager::GetMaterial(path)->GetId(); return true; };
	_materialsIds = LoadResources(pathGetter, idGetter, dataGetter, prefab->_materials, _materialsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_PREFAB_MODELS
	loader = [](const string& path, size_t& id) -> bool { id = ModelsManager::LoadModel(path).GetId(); return true; };
	_modelsIds = LoadResources(pathGetter, idGetter, dataGetter, prefab->_models, _modelsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_PREFAB_PREFABS
	loader = [](const string& path, size_t& id) -> bool {
		Prefab* prefab = PrefabManager::LoadPrefab(path);
		if (prefab != nullptr) {
			id = prefab->GetId();
			return true;
		}
		SPDLOG_ERROR("Couldn't load prefab {0}", path);
		return false;
	};
	_prefabsIds = LoadResources(pathGetter, idGetter, dataGetter, prefab->_prefabs, _prefabsIds, unloader, loader);
#pragma endregion
#pragma region LOADING_SCRIPTABLE_OBJECTS
	ScriptableObjectManager::SceneDeserializationBegin();
	for (unsigned int i = 0; i < prefab->_scriptableObjects.size(); i++)
	{
		size_t id = ScriptableObjectManager::SceneDeserialize(0, prefab->_scriptableObjects[i]);
		if (std::find_if(_scriptableObjectsIds.cbegin(), _scriptableObjectsIds.cend(), [id](const size_t& soId) { return soId == id; }) == _scriptableObjectsIds.end())
		{
			_scriptableObjectsIds.push_back(id);
		}
		//_scriptableObjectsIds.push_back(id);
	}
#pragma endregion
#pragma region LOADING_PREFAB_GAMEOBJECTS
	// GAME OBJECTS

	// CREATE NEW OBJECTS WITH TRANSFORMS
	stack<tuple<GameObject*, size_t, size_t>> idTakenObjects; // obj, from, to
	map<size_t, Component*> prefabComponentsById; // all New Created components
	stack<tuple<Component*, size_t, size_t>> idTakenComponents; // comp, from, to

	Action<GameObject*, size_t> CheckTakenID = [&](GameObject* objToReplace, size_t idToTake) -> void {
		if (_gameObjectsById.find(idToTake) != _gameObjectsById.end()) {
			size_t newId = GameObject::GetFreeId();
			idTakenObjects.push({ _gameObjectsById[idToTake], idToTake, newId });
			_gameObjectsById[newId] = _gameObjectsById[idToTake];
		}
		_gameObjectsById[idToTake] = objToReplace;
	};

	Action<Component*, size_t> CheckComponentTakenID = [&](Component* componentToReplace, size_t idToTake) -> void {
		if (_componentsById.find(idToTake) != _componentsById.end()) {
			size_t newId = Component::GetFreeId();
			idTakenComponents.push({ _componentsById[idToTake], idToTake, newId });
			_componentsById[newId] = _componentsById[idToTake];
		}
		_componentsById[idToTake] = componentToReplace;
		prefabComponentsById[idToTake] = componentToReplace;
	};

	GameObject* prefabRoot = new GameObject(0);
	CheckTakenID(prefabRoot, 0);

	size_t rootTransformId = prefab->_rootObject["transform"]["id"].as<size_t>();
	CheckComponentTakenID(prefabRoot->GetTransform(), rootTransformId);
	prefabRoot->GetTransform()->Init(prefabRoot, rootTransformId);

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
		Component* comp = obj->GetTransform();
		CheckComponentTakenID(comp, transformId);
		comp->Init(obj, transformId);
	}

	// LOAD ROOT OBJECT AND TRANSFORM VALUES
	map<size_t, YAML::Node> componentsNodes;
	// GameObject
	if (!prefabRoot->Deserialize(prefab->_rootObject)) {
		SPDLOG_ERROR("Prefab '{0}' Root GameObject data is corupted", PrefabManager::GetPrefabPath(prefab));
	}

	// Transform
	if (!prefabRoot->GetTransform()->Deserialize(prefab->_rootObject["transform"])) {
		SPDLOG_ERROR("Prefab '{0}' Root GameObject Transform data is corupted", PrefabManager::GetPrefabPath(prefab));
	}

	// Components Node
	componentsNodes[0] = prefab->_rootObject["components"];

	// LOAD GAMEOBJECTS AND TRASFORMS VALUES
	for (const YAML::Node& gameObjectNode : prefab->_gameObjects) {
		// GameObject
		size_t objId = gameObjectNode["id"].as<size_t>();
		GameObject* obj = _gameObjectsById[objId];
		if (!obj->Deserialize(gameObjectNode)) {
			SPDLOG_ERROR("Prefab '{0}' GameObject {1} data is corupted", PrefabManager::GetPrefabPath(prefab), objId);
		}

		// Transform
		Transform* t = obj->GetTransform();
		if (!t->Deserialize(gameObjectNode["transform"])) {
			SPDLOG_ERROR("Prefab '{0}' GameObject {1} Transform data is corupted", PrefabManager::GetPrefabPath(prefab), objId);
		}

		// Components Node
		componentsNodes[obj->Id()] = gameObjectNode["components"];

		// Set Children
		for (const YAML::Node& childNode : gameObjectNode["children"]) {
			size_t id = childNode.as<size_t>();
			if (id == 0) {
				SPDLOG_ERROR("Couldn't set rootObject (id: 0) as Child of gameObject");
				continue;
			}
			if (!_gameObjectsById.contains(id)) {
				SPDLOG_ERROR("Couldn't find gameObject with id: {0}, provided in children list of gameObject {1}", id, obj->Id());
				continue;
			}
			t->AddChild(_gameObjectsById[id]->GetTransform());
		}
	}

	// UPDATE ACTIVE FLAG
	for (auto& item : _gameObjectsById) {
		if (!item.second->GetActiveSelf())
			item.second->UpdateActiveInChildren();
	}

	// CREATE AND ADD COMPONENTS TO OBJECTS
	for (const auto& compPair : componentsNodes) {
		GameObject* obj = _gameObjectsById[compPair.first];
		for (const YAML::Node& componentNode : compPair.second) {
			string type = componentNode["type"].as<string>();
			if (!ComponentsMap::HasComponent(type)) {
				SPDLOG_ERROR("Couldn't find create function for component of type '{0}'", type);
				continue;
			}

			// Create Component of type
			size_t id = componentNode["id"].as<size_t>();
			Component* comp = ComponentsMap::CreateComponent(type);
			CheckComponentTakenID(comp, id);

			// Add Component To GameObject
			obj->AddComponentNoInit(comp);
			comp->Init(obj, id);
		}
	}

	// LOAD COMPONENTS VALUES
	for (const auto& compPair : componentsNodes) {
		GameObject* obj = _gameObjectsById[compPair.first];
		for (const YAML::Node& componentNode : compPair.second) {
			string type = componentNode["type"].as<string>();
			if (!ComponentsMap::HasComponent(type)) {
				SPDLOG_ERROR("Couldn't find create function for component of type '{0}'", type);
				continue;
			}

			size_t compId = componentNode["id"].as<size_t>();
			Component* comp = _componentsById[compId];

			// Fill Component with values
			if (!comp->Deserialize(componentNode)) {
				SPDLOG_ERROR("Prefab '{0}' Component {1} of GameObject {2} data is corupted", PrefabManager::GetPrefabPath(prefab), compId, compPair.first);
			}
		}
	}

	// RETURN IDS TO ORIGINAL OBJECTS AND COMPONENTS
	while (idTakenObjects.size() > 0) {
		tuple<GameObject*, size_t, size_t> takenIdTuple = idTakenObjects.top();

		size_t oldId = get<1>(takenIdTuple);
		size_t newId = get<2>(takenIdTuple);

		_gameObjectsById[newId] = _gameObjectsById[oldId];
		_gameObjectsById[newId]->_id = newId;
		_gameObjectsById[oldId] = get<0>(takenIdTuple);

		idTakenObjects.pop();
	}
	while (idTakenComponents.size() > 0) {
		tuple<Component*, size_t, size_t> takenIdTuple = idTakenComponents.top();

		size_t oldId = get<1>(takenIdTuple);
		size_t newId = get<2>(takenIdTuple);

		_componentsById[newId] = _componentsById[oldId];
		_componentsById[newId]->_id = newId;
		_componentsById[oldId] = get<0>(takenIdTuple);

		idTakenComponents.pop();
	}

	// INIT COMPONENTS
	for (const auto& compPair : prefabComponentsById) {
		compPair.second->Initialize();
	}
#pragma endregion

	for (const auto& compPair : prefabComponentsById) {
		if (compPair.second->_enabled)
		{
			compPair.second->OnEnable();
		}
		else {
			compPair.second->OnDisable();
		}
	}

	ScriptableObjectManager::SceneDeserializationEnd();

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

string SceneManager::GetCurrentScenePath()
{
	return _loadedScenesPaths[_currentSceneId];
}

vector<string> SceneManager::GetAllLoadedScenesNames() {
	vector<string> names = vector<string>();

	names.reserve(_loadedScenes.size());

	size_t i = 0;
	for (auto& scen : _loadedScenesNames) {
		names.insert(names.begin() + (i++), scen.second);
	}

	return names;
}

vector<GameObject*> SceneManager::GetAllGameObjects()
{
	vector<GameObject*> gameObjects;
	gameObjects.reserve(_gameObjectsById.size());
	for (auto& gameObjectPair : _gameObjectsById) {
		gameObjects.push_back(gameObjectPair.second);
	}
	return gameObjects;
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

size_t SceneManager::GetGIF(size_t loadIdx)
{
	return _gifsIds[loadIdx];
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

size_t SceneManager::GetGIFSaveIdx(size_t gifId)
{
	size_t idx = 0;
	for (const auto& gifPair : GIFManager::_paths) {
		if (gifPair.first == gifId) return idx;
		++idx;
	}
	SPDLOG_WARN("GIF '{0}' Not Found", gifId);
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
	for (const auto& matPair : MaterialsManager::_materialsPaths) {
		if (matPair.first == materialId) return idx;
		++idx;
	}
	SPDLOG_WARN("Material '{0}' Not Found", materialId);
	return idx;
}

size_t SceneManager::GetModelSaveIdx(size_t modelId)
{
	size_t idx = 0;
	for (const auto& modelPair : ModelsManager::_modelsPaths) {
		if (std::regex_match(modelPair.second, std::regex("[{]\\w+[_](?:GENERATED)[_]\\d+[}]"))) continue;
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
	DestroyObject(_rootObject);
	_rootObject = nullptr;
	while (_objectsToDestroy.size() > 0) {
		_objectsToDestroy.pop();
	}

	Action<map<size_t, size_t>&, const Action<size_t>&> unloader = [](map<size_t, size_t>& ids, const Action<size_t>& unload) -> void {
		for (auto& id : ids) {
			unload(id.second);
		}
		ids.clear();
	};

	unloader(_texturesIds, [](size_t id) -> void { TextureManager::UnloadTexture2D(id); });
	unloader(_spritesIds, [](size_t id) -> void { SpriteManager::UnloadSprite(id); });
	unloader(_fontsIds, [](size_t id) -> void { FontManager::UnloadFont(id); });
	unloader(_gifsIds, [](size_t id) -> void { GIFManager::Unload(id); });
	unloader(_audiosIds, [](size_t id) -> void { AudioManager::UnloadAudio(id); });
	unloader(_materialsIds, [](size_t id) -> void { MaterialsManager::UnloadMaterial(id); });
	//Twin2Engine::Manager::ShaderManager::UnloadAll();
	unloader(_modelsIds, [](size_t id) -> void { ModelsManager::UnloadModel(id); });
	unloader(_prefabsIds, [](size_t id) -> void { PrefabManager::UnloadPrefab(id); });
	//Twin2Engine::Manager::MeshRenderingManager::UnloadAll();
}

void SceneManager::UnloadScene(const std::string& name)
{
	size_t sceneId = hash<string>()(name);
	if (!_loadedScenes.contains(sceneId)) {
		SPDLOG_INFO("Failed to unload scene - Scene '{0}' not found", name);
		return;
	}

	Scene*& scene = _loadedScenes[sceneId];
	if (sceneId == _currentSceneId) {
		UnloadCurrent();
	}
	delete scene;
	_loadedScenes.erase(sceneId);
	_loadedScenesPaths.erase(sceneId);
	_loadedScenesNames.erase(sceneId);
}

void SceneManager::UnloadAll()
{
	UnloadCurrent();
	for (auto& sceneP : _loadedScenes) {
		delete sceneP.second;
	}
	_loadedScenes.clear();
	_loadedScenesPaths.clear();
	_loadedScenesNames.clear();
}

#if _DEBUG
void SceneManager::DrawCurrentSceneEditor()
{
	if (!ImGui::Begin(SceneManager::GetCurrentSceneName().c_str(), NULL, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	ImGui::PushStyleColor(ImGuiCol_DragDropTarget, ImVec4(0.1f, 0.7f, 0.2f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(0.1f, 0.7f, 0.2f, 1.f));

	bool addPrefab = false;

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(string("Create##").append(SceneManager::GetCurrentSceneName()).c_str()))
		{
			if (ImGui::MenuItem(string("Add GameObject##Create").append(SceneManager::GetCurrentSceneName()).c_str()))
				CreateGameObject();

			if (ImGui::MenuItem(string("Add Prefab GameObject##Create").append(SceneManager::GetCurrentSceneName()).c_str()))
				addPrefab = true;

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (addPrefab) { 
		ImGui::OpenPopup(string("Add Prefab GameObject PopUp##Scene Manager").c_str(), ImGuiPopupFlags_NoReopen); 
		addPrefab = false;
	}

	if (ImGui::BeginPopup(string("Add Prefab GameObject PopUp##Scene Manager").c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {

		map<size_t, string> types = PrefabManager::GetAllPrefabsNames();

		size_t choosed = 0;

		if (types.size() == 0) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "No prefab available!");

		ImGui::BeginDisabled(types.size() == 0);
		if (ImGui::BeginCombo(string("##Scene Manager POP UP Prefab GameObject").c_str(), choosed == 0 ? "None" : types[choosed].c_str())) {

			bool clicked = false;
			size_t i = 0;
			for (auto& item : types) {

				if (ImGui::Selectable(std::string(item.second).append("##Scene Manager POP UP Prefab GameObject").append(std::to_string(i)).c_str(), item.first == choosed)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
				++i;
			}

			if (clicked) {
				if (choosed != 0) {
					CreateGameObject(PrefabManager::GetPrefab(choosed));
				}
			}

			ImGui::EndCombo();
		}
		ImGui::EndDisabled();

		if (choosed != 0) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	DrawGameObjectEditor(_rootObject);

	ImGui::Dummy(ImGui::GetContentRegionAvail());

	if (ImGui::BeginDragDropTarget()) {

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(_payloadType.c_str()))
		{
			size_t payload_n = *(const size_t*)payload->Data;
			Transform* t = SceneManager::GetGameObjectWithId(payload_n)->GetTransform();
			if (_rootObject->GetTransform() != t->GetParent()) {
				t->SetParent(_rootObject->GetTransform());
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::End();
}
#endif