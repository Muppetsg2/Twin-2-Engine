#include <inc/RenderableComponent.h>

using namespace Twin2EngineCore;

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