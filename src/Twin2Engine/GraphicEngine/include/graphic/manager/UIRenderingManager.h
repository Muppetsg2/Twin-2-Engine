#pragma once

#include <tools/STD140Struct.h>

namespace Twin2Engine
{
	namespace Graphic {
		class GraphicEngine;
		class Shader;
	}

	namespace Manager {
		struct UIElement {
			glm::mat4 transform;
			glm::vec4 color;
			glm::vec2 elemSize;
			glm::ivec2 spriteSize;
			glm::ivec2 spriteOffset;
			glm::ivec2 textureSize;
			uint32_t textureID;
			bool isText;
			bool hasTexture;
		};

		static const Tools::STD140Offsets CanvasOffsets{
			Tools::STD140Variable<glm::ivec2>("canvasSize")
		};

		static const Tools::STD140Offsets UIElementOffsets{
			Tools::STD140Variable<glm::mat4>("transform"),
			Tools::STD140Variable<glm::vec4>("color"),
			Tools::STD140Variable<glm::vec2>("elemSize"),
			Tools::STD140Variable<glm::ivec2>("spriteOffset"),
			Tools::STD140Variable<glm::ivec2>("spriteSize"),
			Tools::STD140Variable<glm::ivec2>("texSize"),
			Tools::STD140Variable<bool>("isText"),
			Tools::STD140Variable<bool>("hasTexture")
		};

		static Tools::STD140Struct MakeUIElementStruct(const UIElement& uiElem) {
			Tools::STD140Struct uiElemStruct(UIElementOffsets);
			uiElemStruct.Set("transform", uiElem.transform);
			uiElemStruct.Set("color", uiElem.color);
			uiElemStruct.Set("elemSize", uiElem.elemSize);
			uiElemStruct.Set("spriteOffset", uiElem.spriteOffset);
			uiElemStruct.Set("spriteSize", uiElem.spriteSize);
			uiElemStruct.Set("texSize", uiElem.textureSize);
			uiElemStruct.Set("isText", uiElem.isText);
			uiElemStruct.Set("hasTexture", uiElem.hasTexture);
			return uiElemStruct;
		}

		class UIRenderingManager
		{
		private:
			static std::queue<UIElement> _renderQueue;
			static Graphic::Shader* _uiShader;

			// POINT
			static uint32_t _pointVAO;
			static uint32_t _pointVBO;

			// UBOs
			static uint32_t _canvasUBO;
			static uint32_t _elemUBO;

			static void Init();
			static void UnloadAll();

			static void Render();
		public:
			static void Render(UIElement elem);

			friend class Graphic::GraphicEngine;
		};
	}
}