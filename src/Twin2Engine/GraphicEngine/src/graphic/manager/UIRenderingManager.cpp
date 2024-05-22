#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/Shader.h>
#include <graphic/Window.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Tools;
using namespace std;
using namespace glm;

// STD140 STRUCTS
// STRUCT
STD140Struct UIRenderingManager::RectTransformStruct{
	STD140Variable<mat4>("transform"),
	STD140Variable<vec2>("size")
};

// STRUCT
STD140Struct UIRenderingManager::SpriteStruct{
	STD140Variable<uvec2>("offset"),
	STD140Variable<uvec2>("size"),
	STD140Variable<uvec2>("texSize"),
	STD140Variable<bool>("isActive")
};

// UBO DATA
STD140Struct UIRenderingManager::CanvasStruct{
	STD140Variable<STD140Offsets>("canvasRect", UIRenderingManager::RectTransformStruct.GetOffsets()),
	STD140Variable<bool>("canvasIsInWorldSpace"),
	STD140Variable<bool>("canvasIsActive")
};

// UBO DATA
STD140Struct UIRenderingManager::MaskStruct{
	STD140Variable<STD140Offsets>("maskRect", UIRenderingManager::RectTransformStruct.GetOffsets()),
	STD140Variable<STD140Offsets>("maskSprite", UIRenderingManager::SpriteStruct.GetOffsets()),
	STD140Variable<bool>("maskIsActive")
};

// STRUCT
STD140Struct UIRenderingManager::UIElementStruct{
	STD140Variable<STD140Offsets>("rect", UIRenderingManager::RectTransformStruct.GetOffsets()),
	STD140Variable<STD140Offsets>("sprite", UIRenderingManager::SpriteStruct.GetOffsets()),
	STD140Variable<vec4>("color"),
	STD140Variable<bool>("isText")
};

// UBO DATA
STD140Struct UIRenderingManager::UIElementsBufferStruct{
	STD140Variable<STD140Offsets>("uiElements", UIRenderingManager::UIElementStruct.GetOffsets(), UIRenderingManager::maxUIElementsPerRender)
};

// QUEUE
map<CanvasData*, map<int32_t, map<MaskData*, map<Texture2D*, queue<UIRenderingManager::UIElementQueueData>>>>> UIRenderingManager::_renderQueue;

// SHADER
Shader* UIRenderingManager::_uiShader = nullptr;

// POINT
uint32_t UIRenderingManager::_pointVAO = 0;
uint32_t UIRenderingManager::_pointVBO = 0;

// UBOs
uint32_t UIRenderingManager::_canvasUBO = 0;
uint32_t UIRenderingManager::_maskUBO = 0;

// SSBO
uint32_t UIRenderingManager::_elemsSSBO = 0;

void UIRenderingManager::Init() {
	_uiShader = ShaderManager::GetShaderProgram("origin/UI");
	
	// CANVAS UBO
	glGenBuffers(1, &_canvasUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _canvasUBO);
	glBufferData(GL_UNIFORM_BUFFER, CanvasStruct.GetSize(), CanvasStruct.GetData().data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, _canvasUBO);

	// MASK UBO
	glGenBuffers(1, &_maskUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _maskUBO);
	glBufferData(GL_UNIFORM_BUFFER, MaskStruct.GetSize(), MaskStruct.GetData().data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, _maskUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, NULL);

	// UI ELEMENTS SSBO
	glGenBuffers(1, &_elemsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _elemsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, UIElementsBufferStruct.GetSize(), UIElementsBufferStruct.GetData().data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _elemsSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, NULL);

	// Point Buffer
	glGenVertexArrays(1, &_pointVAO);
	glGenBuffers(1, &_pointVBO);

	glBindVertexArray(_pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _pointVBO);

	glm::vec2 point(0.f);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), &point, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void UIRenderingManager::UnloadAll() {
	_renderQueue.clear();
	_uiShader = nullptr;

	glDeleteBuffers(1, &_elemsSSBO);
	glDeleteBuffers(1, &_maskUBO);
	glDeleteBuffers(1, &_canvasUBO);
	glDeleteBuffers(1, &_pointVBO);
	glDeleteVertexArrays(1, &_pointVAO);
}

void UIRenderingManager::Render()
{
#if TRACY_PROFILER
	const char* const tracy_RenderUIShader = "Render UI Shader";
	char* const tracy_RenderUICanvasName = new char[50];
	char* const tracy_RenderUICanvasUBOName = new char[50];
	char* const tracy_RenderUILayerName = new char[50];
	char* const tracy_RenderUIMaskName = new char[50];
	char* const tracy_RenderUIMaskUBOName = new char[50];
	char* const tracy_RenderUITextureName = new char[50];
	char* const tracy_RenderUIElementName = new char[50];
	char* const tracy_RenderUIElementDataName = new char[50];
	const char* const tracy_RenderUIEnd = "Render UI End";

	ZoneScoped;
#endif

	if (_uiShader != nullptr) {
#if TRACY_PROFILER
		FrameMarkStart(tracy_RenderUIShader);
#endif

		_uiShader->Use();
		_uiShader->SetInt("image", 0);
		_uiShader->SetInt("maskImage", 1);

#if TRACY_PROFILER
		FrameMarkEnd(tracy_RenderUIShader);
#endif

		glBindVertexArray(_pointVAO);
		size_t canvasId = 0;
		for (const auto& canvas : _renderQueue) {

#if TRACY_PROFILER
			sprintf(tracy_RenderUICanvasName, "Render UI Canvas %zu", canvasId);
			FrameMarkStart(tracy_RenderUICanvasName);
#endif
			
			CanvasData* canvasData = canvas.first;

#if TRACY_PROFILER
			sprintf(tracy_RenderUICanvasUBOName, "Render UI Canvas UBO %zu", canvasId);
			FrameMarkStart(tracy_RenderUICanvasUBOName);
#endif

			glBindBuffer(GL_UNIFORM_BUFFER, _canvasUBO);
			if (canvasData != nullptr) {
				RectTransformStruct.Set("transform", canvasData->rectTransform.transform);
				RectTransformStruct.Set("size", canvasData->rectTransform.transform);

				CanvasStruct.Set("canvasRect", RectTransformStruct);
				CanvasStruct.Set("canvasIsInWorldSpace", canvasData->worldSpaceCanvas);
			}
			CanvasStruct.Set("canvasIsActive", canvasData != nullptr);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, CanvasStruct.GetSize(), CanvasStruct.GetData().data());

#if TRACY_PROFILER
			FrameMarkEnd(tracy_RenderUICanvasUBOName);
#endif

			for (const auto& layer : canvas.second) {

#if TRACY_PROFILER
				sprintf(tracy_RenderUILayerName, "Render UI Layer %d", layer.first);
				FrameMarkStart(tracy_RenderUILayerName);
#endif

				size_t maskId = 0;
				for (const auto& mask : layer.second) {

#if TRACY_PROFILER
					sprintf(tracy_RenderUIMaskName, "Render UI Mask %zu", maskId);
					FrameMarkStart(tracy_RenderUIMaskName);
#endif

					MaskData* maskData = mask.first;

#if TRACY_PROFILER
					sprintf(tracy_RenderUIMaskUBOName, "Render UI Mask UBO %zu", maskId);
					FrameMarkStart(tracy_RenderUIMaskUBOName);
#endif

					glBindBuffer(GL_UNIFORM_BUFFER, _maskUBO);
					if (maskData != nullptr) {
						RectTransformStruct.Set("transform", maskData->rectTransform.transform);
						RectTransformStruct.Set("size", maskData->rectTransform.size);
						
						MaskStruct.Set("maskRect", RectTransformStruct);
						
						if (maskData->maskSprite != nullptr) {
							SpriteStruct.Set("offset", maskData->maskSprite->GetOffset());
							SpriteStruct.Set("size", maskData->maskSprite->GetSize());
							SpriteStruct.Set("texSize", maskData->maskSprite->GetTexture()->GetSize());

							MaskStruct.Set("maskSprite", SpriteStruct);

							maskData->maskSprite->Use(1);
						}
						MaskStruct.Set("maskSprite.isActive", maskData->maskSprite != nullptr);
					}
					MaskStruct.Set("maskIsActive", maskData != nullptr);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, MaskStruct.GetSize(), MaskStruct.GetData().data());

#if TRACY_PROFILER
					FrameMarkEnd(tracy_RenderUIMaskUBOName);
#endif

					size_t textureId = 0;
					for (const auto& texture : mask.second) {
#if TRACY_PROFILER
						sprintf(tracy_RenderUITextureName, "Render UI Texture %zu", textureId);
						FrameMarkStart(tracy_RenderUITextureName);
#endif

						Texture2D* textureData = texture.first;

						glBindBuffer(GL_SHADER_STORAGE_BUFFER, _elemsSSBO);

						if (textureData != nullptr) {
							textureData->Use(0);
						}

						queue<UIElementQueueData> renderQueue = texture.second;
						size_t i = 0;
						size_t elementId = 0;
						while (renderQueue.size() > 0) {
#if TRACY_PROFILER
							sprintf(tracy_RenderUIElementName, "Render UI Element %zu", elementId);
							FrameMarkStart(tracy_RenderUIElementName);
#endif

							const UIElementQueueData& uiElem = renderQueue.front();

#if TRACY_PROFILER
							sprintf(tracy_RenderUIElementDataName, "Render UI Element Data %zu", elementId);
							FrameMarkStart(tracy_RenderUIElementDataName);
#endif

							RectTransformStruct.Set("transform", uiElem.rectTransform.transform);
							RectTransformStruct.Set("size", uiElem.rectTransform.size);

							UIElementStruct.Set("rect", RectTransformStruct);
							UIElementStruct.Set("color", uiElem.color);
							UIElementStruct.Set("isText", uiElem.isText);
							
							if (textureData != nullptr) {
								SpriteStruct.Set("offset", uiElem.spriteOffset);
								SpriteStruct.Set("size", uiElem.spriteSize);
								SpriteStruct.Set("texSize", textureData->GetSize());

								UIElementStruct.Set("sprite", SpriteStruct);
							}
							UIElementStruct.Set("sprite.isActive", textureData != nullptr);

#if TRACY_PROFILER
							FrameMarkEnd(tracy_RenderUIElementDataName);
#endif

							UIElementsBufferStruct.Set("uiElements[" + to_string(i) + "]", UIElementStruct);

							if (++i == maxUIElementsPerRender) {
								glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, UIElementsBufferStruct.GetSize(), UIElementsBufferStruct.GetData().data());
								glDrawArraysInstanced(GL_POINTS, 0, 1, maxUIElementsPerRender);
								i = 0;
							}

							renderQueue.pop();

#if TRACY_PROFILER
							FrameMarkEnd(tracy_RenderUIElementName);
#endif

							++elementId;
						}

						if (i != 0) {
							glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, UIElementsBufferStruct.GetSize(), UIElementsBufferStruct.GetData().data());
							glDrawArraysInstanced(GL_POINTS, 0, 1, i);
						}

#if TRACY_PROFILER
						FrameMarkEnd(tracy_RenderUITextureName);
#endif

						++textureId;
					}

#if TRACY_PROFILER
					FrameMarkEnd(tracy_RenderUIMaskName);
#endif

					++maskId;
				}

#if TRACY_PROFILER
				FrameMarkEnd(tracy_RenderUILayerName);
#endif
			}

#if TRACY_PROFILER
			FrameMarkEnd(tracy_RenderUICanvasName);
#endif

			++canvasId;
		}

#if TRACY_PROFILER
		FrameMarkStart(tracy_RenderUIEnd);
#endif

		_renderQueue.clear();
		glBindBuffer(GL_UNIFORM_BUFFER, NULL);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, NULL);
		glBindVertexArray(NULL);

#if TRACY_PROFILER
		FrameMarkEnd(tracy_RenderUIEnd);
#endif
	}

#if TRACY_PROFILER
	delete[] tracy_RenderUICanvasName;
	delete[] tracy_RenderUICanvasUBOName;
	delete[] tracy_RenderUILayerName;
	delete[] tracy_RenderUIMaskName;
	delete[] tracy_RenderUIMaskUBOName;
	delete[] tracy_RenderUITextureName;
	delete[] tracy_RenderUIElementName;
	delete[] tracy_RenderUIElementDataName;
#endif
}

void UIRenderingManager::Render(UITextData text) 
{
	_renderQueue[text.canvas][text.layer][text.mask][text.charTexture].push(UIElementQueueData{
		.rectTransform = text.rectTransform,
		.color = text.color,
		.spriteSize = text.charTexture->GetSize(),
		.spriteOffset = uvec2(0.f),
		.isText = true
	});
}

void UIRenderingManager::Render(UIImageData image)
{
	_renderQueue[image.canvas][image.layer][image.mask][image.sprite->GetTexture()].push(UIElementQueueData{
		.rectTransform = image.rectTransform,
		.color = image.color,
		.spriteSize = image.sprite->GetSize(),
		.spriteOffset = image.sprite->GetOffset(),
		.isText = false
	});
}