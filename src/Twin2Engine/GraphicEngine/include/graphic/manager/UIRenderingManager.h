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
			glm::mat4 canvasTransform = glm::mat4(1.f);
			glm::mat4 elemTransform = glm::mat4(1.f);
			glm::mat4 maskTransform = glm::mat4(1.f);
			glm::vec4 color = glm::vec4(0.f);
			glm::vec2 canvasSize = glm::vec2(0.f);
			glm::vec2 elemSize = glm::vec2(0.f);
			glm::vec2 maskSize = glm::vec2(0.f);
			glm::ivec2 spriteSize = glm::ivec2(0);
			glm::ivec2 spriteOffset = glm::ivec2(0);
			glm::ivec2 textureSize = glm::ivec2(0);
			uint32_t textureID = 0;
			uint32_t maskTextureID = 0;
			bool isText = false;
			bool hasTexture = false;
			bool worldSpaceCanvas = false;
			bool hasMaskTexture = false;
			bool useMask = false;
		};

		static const Tools::STD140Offsets CanvasOffsets{
			Tools::STD140Variable<glm::mat4>("canvasTransform"),
			Tools::STD140Variable<glm::vec2>("canvasSize"),
			Tools::STD140Variable<bool>("worldSpaceCanvas")
		};

		static const Tools::STD140Offsets UIElementOffsets{
			Tools::STD140Variable<glm::mat4>("elemTransform"),
			Tools::STD140Variable<glm::mat4>("maskTransform"),
			Tools::STD140Variable<glm::vec4>("color"),
			Tools::STD140Variable<glm::vec2>("elemSize"),
			Tools::STD140Variable<glm::vec2>("maskSize"),
			Tools::STD140Variable<glm::ivec2>("spriteOffset"),
			Tools::STD140Variable<glm::ivec2>("spriteSize"),
			Tools::STD140Variable<glm::ivec2>("texSize"),
			Tools::STD140Variable<bool>("isText"),
			Tools::STD140Variable<bool>("hasTexture"),
			Tools::STD140Variable<bool>("hasMaskTexture"),
			Tools::STD140Variable<bool>("useMask")
		};

		static Tools::STD140Struct MakeUIElementStruct(const UIElement& uiElem) {
			Tools::STD140Struct uiElemStruct(UIElementOffsets);
			uiElemStruct.Set("elemTransform", uiElem.elemTransform);
			uiElemStruct.Set("maskTransform", uiElem.maskTransform);
			uiElemStruct.Set("color", uiElem.color);
			uiElemStruct.Set("elemSize", uiElem.elemSize);
			uiElemStruct.Set("maskSize", uiElem.maskSize);
			uiElemStruct.Set("spriteOffset", uiElem.spriteOffset);
			uiElemStruct.Set("spriteSize", uiElem.spriteSize);
			uiElemStruct.Set("texSize", uiElem.textureSize);
			uiElemStruct.Set("isText", uiElem.isText);
			uiElemStruct.Set("hasTexture", uiElem.hasTexture);
			uiElemStruct.Set("hasMaskTexture", uiElem.hasMaskTexture);
			uiElemStruct.Set("useMask", uiElem.useMask);
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