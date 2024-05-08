#pragma once

#include <GraphicEnigineManager.h>
#include <graphic/InstantiatingModel.h>
#include <graphic/Shader.h>
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
			static GraphicEngine::InstantiatingModel _spritePlane;
			static GraphicEngine::Shader* _uiShader;


			static void Init();

			static void Render();
		public:
			static void Render(UIElement elem);

			friend class GraphicEngine::GraphicEngineManager;
		};
	}
}