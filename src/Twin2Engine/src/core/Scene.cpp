#include <core/Scene.h>
#include <core/RenderableComponent.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/FontManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

Scene::Scene() {
	_rootObject = new GameObject(this);
}

Scene::~Scene() {
	DeleteGameObject(_rootObject);
}

void Scene::DeleteGameObject(GameObject* obj)
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

GameObject* Scene::AddGameObject()
{
	GameObject* obj = new GameObject(this);
	_rootObject->GetTransform()->AddChild(obj->GetTransform());
	return obj;
}

void Scene::AddTexture(const string& path) {
	_textures.push_back(path);
}