#include <core/RenderableComponent.h>

using namespace Twin2Engine::Core;

std::vector<RenderableComponent*> RenderableComponent::renderableComponents = std::vector<RenderableComponent*>();

RenderableComponent::RenderableComponent()
{
	renderableComponents.push_back(this);
}

RenderableComponent::~RenderableComponent()
{
	for (size_t i = 0; i < renderableComponents.size(); ++i) {
		if (renderableComponents[i] == this) {
			renderableComponents.erase(renderableComponents.begin() + i);
			break;
		}
	}
}

void RenderableComponent::Render()
{
}

bool Twin2Engine::Core::RenderableComponent::IsTransparent() const
{
	return _isTransparent;
}

void Twin2Engine::Core::RenderableComponent::SetIsTransparent(bool value)
{
	_isTransparent = value;
}