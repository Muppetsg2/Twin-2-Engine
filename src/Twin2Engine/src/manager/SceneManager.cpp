#include <manager/SceneManager.h>
#include <core/RenderableComponent.h>

using namespace Twin2Engine::Manager;
using namespace std;
using namespace Twin2Engine::Core;

Scene* SceneManager::_currentScene = nullptr;
GameObject* SceneManager::_rootObject = nullptr;

vector<size_t> SceneManager::_texturesIds = vector<size_t>();
vector<size_t> SceneManager::_spritesIds = vector<size_t>();
vector<size_t> SceneManager::_fontsIds = vector<size_t>();

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

	// TEXTURES
	vector<string> paths;
	for (auto& path : sceneToLoad->_textures) {
		paths.push_back(path.first);
	}

	auto toLoadToUnload = GetResourcesToLoadAndUnload(paths, _texturesIds);
	
	for (auto& t : toLoadToUnload.second) {
		TextureManager::UnloadTexture2D(t);
	}
	for (auto& t : toLoadToUnload.first) {
		string path = paths[t];
		TextureManager::LoadTexture2D(path, sceneToLoad->_textures[path]);
	}

	// SPRITES
	paths.clear();
	for (auto& path : sceneToLoad->_sprites) {
		paths.push_back(path.first);
	}

	toLoadToUnload = GetResourcesToLoadAndUnload(paths, _spritesIds);

	for (auto& s : toLoadToUnload.second) {
		SpriteManager::UnloadSprite(s);
	}
	for (auto& s : toLoadToUnload.first) {
		string alias = paths[s];
		tuple<string, bool, SpriteData> sData = sceneToLoad->_sprites[alias];
		if (get<1>(sData)) {
			SpriteManager::MakeSprite(alias, get<0>(sData), get<2>(sData));
		}
		else {
			SpriteManager::MakeSprite(alias, get<0>(sData));
		}
	}

	// FONTS
	toLoadToUnload = GetResourcesToLoadAndUnload(sceneToLoad->_fonts, _fontsIds);

	for (auto& f : toLoadToUnload.second) {
		FontManager::UnloadFont(f);
	}
	for (auto& f : toLoadToUnload.first) {
		FontManager::LoadFont(paths[f]);
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