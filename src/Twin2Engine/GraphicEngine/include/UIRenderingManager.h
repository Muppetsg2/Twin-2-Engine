#pragma once

#include <vector>
#include <core/RenderableComponent.h>

namespace Twin2Engine::GraphicEngine
{
	class UIRenderingManager
	{
	private:
		static std::vector<Core::RenderableComponent*> _uiComponents;

	public:
		static void Register(Core::RenderableComponent* uiObject);
		static void Unregister(Core::RenderableComponent* uiObject);

		static void Render(const Core::Window* window);
	};
}