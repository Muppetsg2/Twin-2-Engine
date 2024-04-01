#pragma once

#include <graphic/Window.h>
#include <graphic/Sprite.h>

namespace Twin2Engine::GraphicEngine
{
	class GraphicEngine;

	struct UIElement {
		uint32_t textureID;
		glm::ivec2 spriteSize;
		glm::ivec2 spriteOffset;
		glm::ivec2 textureSize;
		glm::vec4 color;
		glm::vec2 elemSize;
		glm::mat4 transform;
	};

	class UIRenderingManager
	{
	private:
		static std::queue<UIElement> _renderQueue;

		static void Render(const Window* window);
	public:
		static void Render(UIElement elem);

		friend class GraphicEngine;
	};
}