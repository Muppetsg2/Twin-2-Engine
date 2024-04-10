#include <core/Scene.h>
#include <core/RenderableComponent.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;
using namespace std;

void Scene::AddGameObject(const YAML::Node& gameObjectNode)
{
	_gameObjects.push_back(gameObjectNode);
}

void Scene::AddTexture2D(const string& path, const TextureData& data) {
	_textures[path] = data;
}

void Scene::AddSprite(const string& name, const string& texPath)
{
	_sprites[name] = tuple<string, bool, SpriteData>(texPath, false, SpriteData());

	bool hasTex = false;
	for (auto& tex : _textures) {
		if (tex.first == texPath) {
			hasTex = true;
			break;
		}
	}

	if (!hasTex) {
		AddTexture2D(texPath);
	}
}

void Scene::AddSprite(const string& name, const string& texPath, const SpriteData& data)
{
	_sprites[name] = tuple<string, bool, SpriteData>(texPath, true, data);

	bool hasTex = false;
	for (auto& tex : _textures) {
		if (tex.first == texPath) {
			hasTex = true;
			break;
		}
	}

	if (!hasTex) {
		AddTexture2D(texPath);
	}
}

void Scene::AddFont(const string& path)
{
	_fonts.push_back(path);
	_fonts.unique();
}

void Scene::AddAudio(const std::string& path)
{
	_audios.push_back(path);
	_audios.unique();
}