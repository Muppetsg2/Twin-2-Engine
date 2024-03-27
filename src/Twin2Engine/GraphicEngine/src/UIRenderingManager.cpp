#include <UIRenderingManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Core;
using namespace std;

vector<RenderableComponent*> UIRenderingManager::_uiComponents = vector<RenderableComponent*>();

void UIRenderingManager::Register(RenderableComponent* uiObject)
{
	_uiComponents.push_back(uiObject);
}

void UIRenderingManager::Unregister(RenderableComponent* uiObject)
{
	std::remove_if(_uiComponents.begin(), _uiComponents.end(), [&](auto& p) -> bool { return p == uiObject; });
}

void UIRenderingManager::Render(const Window* window)
{
	for (RenderableComponent* comp : _uiComponents) {
		comp->Render(window);
	}
}