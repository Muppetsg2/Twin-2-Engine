#pragma once

#include <graphic/InstatiatingModel.h>
#include <graphic/Shader.h>
#include <graphic/Window.h>

namespace Twin2Engine
{
	namespace Graphic {
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
			static Graphic::InstatiatingModel _spritePlane;
			static Graphic::Shader* _uiShader;


			static void Init();
			static void UnloadAll();

			static void Render();
		public:
			static void Render(UIElement elem);

			friend class Graphic::GraphicEngine;
		};
	}
}