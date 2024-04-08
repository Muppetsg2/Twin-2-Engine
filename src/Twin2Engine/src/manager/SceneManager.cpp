#include <manager/SceneManager.h>
#include <graphic/manager/TextureManager.h>
#include <core/RenderableComponent.h>

using namespace Twin2Engine::Manager;
using namespace std;
using namespace Twin2Engine::Core;

Scene* SceneManager::_currentScene = nullptr;
vector<size_t> SceneManager::_texturesIds = vector<size_t>();
GameObject* SceneManager::_rootObject = nullptr;
map<size_t, Scene*> SceneManager::_loadedScenes = map<size_t, Scene*>();

void SceneManager::AddScene(const string& name, Scene* scene)
{
	size_t id = hash<string>()(name);
	if (_loadedScenes.find(id) != _loadedScenes.end()) {
		SPDLOG_WARN("Zastêpowanie Sceny o nazwie '{0}'", name);
	}
	_loadedScenes[id] = scene;
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

	vector<size_t> sceneToLoadTextureHashes;
	vector<string> sceneToLoadTexturePaths;
	for (auto& path : sceneToLoad->_textures) {
		sceneToLoadTexturePaths.push_back(path.first);
		sceneToLoadTextureHashes.push_back(hasher(path.first));
	}

	vector<size_t> toUnload, toLoad;
	for (size_t i = 0; i < sceneToLoadTexturePaths.size(); ++i) {
		toLoad.push_back(i);
	}

	for (auto& h : _texturesIds) {
		bool hasHash = false;
		for (auto& h2 : sceneToLoadTextureHashes) {
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
				if (sceneToLoadTextureHashes[toLoad[i]] == h) {
					toLoad.erase(toLoad.begin() + i);
				}
			}
		}
	}
	
	for (auto& t : toUnload) {
		TextureManager::UnloadTexture2D(t);
	}
	for (auto& t : toLoad) {
		string path = sceneToLoadTexturePaths[t];
		TextureManager::LoadTexture2D(path, sceneToLoad->_textures[path]);
	}

	_rootObject = sceneToLoad->_rootObject;
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
		if (comp->GetGameObject()->GetObjectScene() == _currentScene) {
			comp->Render();
		}
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