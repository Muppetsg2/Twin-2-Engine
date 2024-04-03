#pragma once

#include <graphic/Window.h>

namespace Twin2Engine
{
	namespace GraphicEngine {
		class GraphicEngine;
	}

	namespace Manager {
		struct UIElement {
			uint32_t textureID;
			bool isText;
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

			static void Render(const GraphicEngine::Window* window);
		public:
			static void Render(UIElement elem);

			friend class GraphicEngine::GraphicEngine;
		};
	}
}