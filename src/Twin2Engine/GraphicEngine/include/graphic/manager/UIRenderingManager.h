#pragma once

#include <tools/STD140Struct.h>
#include <graphic/Sprite.h>

namespace Twin2Engine
{
	namespace Graphic {
		class GraphicEngine;
		class Shader;
	}

	namespace Manager {
		struct UIRectData {
			glm::mat4 transform = glm::mat4(1.f);
			glm::vec2 size = glm::vec2(0.f);
		};

		struct CanvasData {
			UIRectData rectTransform;
			bool worldSpaceCanvas = false;
		};

		struct MaskData {
			UIRectData rectTransform;
			Graphic::Sprite* maskSprite = nullptr;
		};

		struct UIElementData {
			CanvasData* canvas = nullptr;
			MaskData* mask = nullptr;
			
			UIRectData rectTransform;
			glm::vec4 color = glm::vec4(0.f);
			int32_t layer = 0;
		};

		struct UIImageData : public UIElementData {
			Graphic::Sprite* sprite = nullptr;
		};

		struct UITextData : public UIElementData {
			Graphic::Texture2D* charTexture = nullptr;
		};

		class UIRenderingManager
		{
		private:
			static constexpr const uint32_t maxUIElementsPerRender = 8;

			struct UIElementQueueData {
				UIRectData rectTransform;
				glm::vec4 color = glm::vec4(0.f);
				glm::ivec2 spriteSize = glm::ivec2(0);
				glm::ivec2 spriteOffset = glm::ivec2(0);
				bool isText = false;
			};

			// STD140 STRUCT OFFSETS
			static const Tools::STD140Offsets RectTransformOffsets;
			static const Tools::STD140Offsets SpriteOffsets;
			static const Tools::STD140Offsets CanvasOffsets;
			static const Tools::STD140Offsets MaskOffsets;
			static const Tools::STD140Offsets UIElementOffsets;
			static const Tools::STD140Offsets UIElementsBufferOffsets;

			// Canvas -> Layer -> Mask -> Texture -> queue
			static std::map<CanvasData*, std::map<int32_t, std::map<MaskData*, std::map<Graphic::Texture2D*, std::queue<UIElementQueueData>>>>> _renderQueue;
			
			// SHADER
			static Graphic::Shader* _uiShader;

			// POINT
			static uint32_t _pointVAO;
			static uint32_t _pointVBO;

			// UBOs
			static uint32_t _canvasUBO;
			static uint32_t _maskUBO;

			// SSBO
			static uint32_t _elemsSSBO;

			static void Init();
			static void UnloadAll();

			static void Render();
		public:
			static void Render(UITextData textData);
			static void Render(UIImageData imageData);

			friend class Graphic::GraphicEngine;
		};
	}
}