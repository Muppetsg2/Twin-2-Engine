#include <core/RenderableComponent.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;

std::vector<RenderableComponent*> RenderableComponent::_components = std::vector<RenderableComponent*>();

RenderableComponent::RenderableComponent()
{
	_components.push_back(this);
}

RenderableComponent::~RenderableComponent()
{
	for (size_t i = 0; i < _components.size(); ++i) {
		if (_components[i] == this) {
			_components.erase(_components.begin() + i);
			break;
		}
	}
}

void RenderableComponent::Render()
{
}

bool RenderableComponent::IsTransparent() const
{
	return _isTransparent;
}

void RenderableComponent::SetIsTransparent(bool value)
{
	_isTransparent = value;
}