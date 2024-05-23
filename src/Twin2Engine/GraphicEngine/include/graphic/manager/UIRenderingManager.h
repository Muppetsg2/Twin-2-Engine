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
				Graphic::Sprite* sprite = nullptr;
				glm::vec4 color = glm::vec4(0.f);
				bool isText = false;
			};

			// STD140 OFFSETS
			static Tools::STD140Offsets RectTransformOffsets;
			static Tools::STD140Offsets SpriteOffsets;
			static Tools::STD140Offsets TextureOffsets;
			static Tools::STD140Offsets UIElementOffsets;

			// STD140 STRUCTS
			static Tools::STD140Struct CanvasStruct;
			static Tools::STD140Struct MaskStruct;
			static Tools::STD140Struct UIElementsBufferStruct;

			// Canvas -> Layer -> Mask -> Texture -> queue
			static std::unordered_map<CanvasData*, std::map<int32_t, std::unordered_map<MaskData*, std::unordered_map<Graphic::Texture2D*, std::queue<UIElementQueueData>>>>> _renderQueue;
			
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

			// FORMAT
			static const char* const _uiBufforElemFormat;

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