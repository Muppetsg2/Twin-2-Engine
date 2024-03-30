#include <core/RenderableComponent.h>

using namespace Twin2Engine::Core;

void RenderableComponent::Render(const Window* window)
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