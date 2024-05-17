#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/Shader.h>
#include <graphic/Window.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Tools;
using namespace std;
using namespace glm;

// STD140 STRUCT OFFSETS
// STRUCT
const STD140Offsets UIRenderingManager::RectTransformOffsets{
	STD140Variable<mat4>("transform"),
	STD140Variable<vec2>("size")
};

// STRUCT
const STD140Offsets UIRenderingManager::SpriteOffsets{
	STD140Variable<uvec2>("offset"),
	STD140Variable<uvec2>("size"),
	STD140Variable<uvec2>("texSize"),
	STD140Variable<bool>("isActive")
};

// UBO DATA
const STD140Offsets UIRenderingManager::CanvasOffsets{
	STD140Variable<STD140Offsets>("canvasRect", UIRenderingManager::RectTransformOffsets),
	STD140Variable<bool>("canvasIsInWorldSpace"),
	STD140Variable<bool>("canvasIsActive")
};

// UBO DATA
const STD140Offsets UIRenderingManager::MaskOffsets{
	STD140Variable<STD140Offsets>("maskRect", UIRenderingManager::RectTransformOffsets),
	STD140Variable<STD140Offsets>("maskSprite", UIRenderingManager::SpriteOffsets),
	STD140Variable<bool>("maskIsActive")
};

// STRUCT
const STD140Offsets UIRenderingManager::UIElementOffsets{
	STD140Variable<STD140Offsets>("rect", UIRenderingManager::RectTransformOffsets),
	STD140Variable<STD140Offsets>("sprite", UIRenderingManager::SpriteOffsets),
	STD140Variable<vec4>("color"),
	STD140Variable<bool>("isText"),
};

// UBO DATA
const STD140Offsets UIRenderingManager::UIElementsBufferOffsets{
	STD140Variable<STD140Offsets>("uiElements", UIRenderingManager::UIElementOffsets, UIRenderingManager::maxUIElementsPerRender)
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
	glBufferData(GL_UNIFORM_BUFFER, CanvasOffsets.GetSize(), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, _canvasUBO);

	// MASK UBO
	glGenBuffers(1, &_maskUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _maskUBO);
	glBufferData(GL_UNIFORM_BUFFER, MaskOffsets.GetSize(), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, _maskUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, NULL);

	// UI ELEMENTS SSBO
	glGenBuffers(1, &_elemsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _elemsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, UIElementsBufferOffsets.GetSize(), NULL, GL_STATIC_DRAW);
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

const char* const tracy_RenderUIShader = "Render UI Shader";
const char* const tracy_RenderUIInitStructs = "Render UI Init Structs";
char* const tracy_RenderUICanvasName = new char[25];
char* const tracy_RenderUICanvasUBOName = new char[29];
char* const tracy_RenderUILayerName = new char[23];
char* const tracy_RenderUIMaskName = new char[23];
char* const tracy_RenderUIMaskUBOName = new char[27];
char* const tracy_RenderUITextureName = new char[26];
char* const tracy_RenderUIElementName = new char[26];
char* const tracy_RenderUIElementDataName = new char[31];
const char* const tracy_RenderUIEnd = "Render UI End";

void UIRenderingManager::Render()
{
	ZoneScoped;

	if (_uiShader != nullptr) {
		FrameMarkStart(tracy_RenderUIShader);
		_uiShader->Use();
		_uiShader->SetInt("image", 0);
		_uiShader->SetInt("maskImage", 1);
		FrameMarkEnd(tracy_RenderUIShader);

		// INIT STRUCTS
		FrameMarkStart(tracy_RenderUIInitStructs);
		STD140Struct elementsBuffer(UIElementsBufferOffsets);
		STD140Struct canvasStruct(CanvasOffsets);
		STD140Struct maskStruct(MaskOffsets);
		STD140Struct spriteStruct(SpriteOffsets);
		STD140Struct element(UIElementOffsets);
		FrameMarkEnd(tracy_RenderUIInitStructs);

		glBindVertexArray(_pointVAO);
		size_t canvasId = 0;
		for (const auto& canvas : _renderQueue) {
			snprintf(tracy_RenderUICanvasName, 25, "Render UI Canvas %zu", canvasId);
			FrameMarkStart(tracy_RenderUICanvasName);
			
			CanvasData* canvasData = canvas.first;

			snprintf(tracy_RenderUICanvasUBOName, 29, "Render UI Canvas UBO %zu", canvasId);
			FrameMarkStart(tracy_RenderUICanvasUBOName);
			glBindBuffer(GL_UNIFORM_BUFFER, _canvasUBO);
			if (canvasData != nullptr) {
				canvasStruct.Set("canvasRect.transform", canvasData->rectTransform.transform);
				canvasStruct.Set("canvasRect.size", canvasData->rectTransform.size);
				canvasStruct.Set("canvasIsInWorldSpace", canvasData->worldSpaceCanvas);
				canvasStruct.Set("canvasIsActive", true);
			}
			else {
				canvasStruct.Set("canvasIsActive", false);
			}
			glBufferSubData(GL_UNIFORM_BUFFER, 0, canvasStruct.GetSize(), canvasStruct.GetData().data());
			FrameMarkEnd(tracy_RenderUICanvasUBOName);

			for (const auto& layer : canvas.second) {
				snprintf(tracy_RenderUILayerName, 23, "Render UI Layer %d", layer.first);
				FrameMarkStart(tracy_RenderUILayerName);

				size_t maskId = 0;
				for (const auto& mask : layer.second) {
					snprintf(tracy_RenderUIMaskName, 23, "Render UI Mask %zu", maskId);
					FrameMarkStart(tracy_RenderUIMaskName);

					MaskData* maskData = mask.first;

					snprintf(tracy_RenderUIMaskUBOName, 27, "Render UI Mask UBO %zu", maskId);
					FrameMarkStart(tracy_RenderUIMaskUBOName);
					glBindBuffer(GL_UNIFORM_BUFFER, _maskUBO);
					if (maskData != nullptr) {
						maskStruct.Set("maskRect.transform", maskData->rectTransform.transform);
						maskStruct.Set("maskRect.size", maskData->rectTransform.size);
						
						if (maskData->maskSprite != nullptr) {
							spriteStruct.Set("offset", maskData->maskSprite->GetOffset());
							spriteStruct.Set("size", maskData->maskSprite->GetSize());
							spriteStruct.Set("texSize", maskData->maskSprite->GetTexture()->GetSize());
							maskStruct.Set("maskSprite", spriteStruct);

							maskData->maskSprite->Use(1);
						}
						maskStruct.Set("maskSprite.isActive", false);
						glBufferSubData(GL_UNIFORM_BUFFER, 0, maskStruct.GetSize(), maskStruct.GetData().data());
					}
					else {
						bool falseBool = false;
						glBufferSubData(GL_UNIFORM_BUFFER, MaskOffsets.Get("maskIsActive"), sizeof(bool), &falseBool);
					}
					FrameMarkEnd(tracy_RenderUIMaskUBOName);

					size_t textureId = 0;
					for (const auto& texture : mask.second) {
						snprintf(tracy_RenderUITextureName, 26, "Render UI Texture %zu", textureId);
						FrameMarkStart(tracy_RenderUITextureName);

						Texture2D* textureData = texture.first;

						glBindBuffer(GL_SHADER_STORAGE_BUFFER, _elemsSSBO);

						if (textureData != nullptr) {
							textureData->Use(0);
						}

						queue<UIElementQueueData> renderQueue = texture.second;
						size_t i = 0;
						size_t elementId = 0;
						while (renderQueue.size() > 0) {
							snprintf(tracy_RenderUIElementName, 26, "Render UI Element %zu", elementId);
							FrameMarkStart(tracy_RenderUIElementName);

							const UIElementQueueData& uiElem = renderQueue.front();

							snprintf(tracy_RenderUIElementDataName, 31, "Render UI Element Data %zu", elementId);
							FrameMarkStart(tracy_RenderUIElementDataName);
							element.Set("rect.transform", uiElem.rectTransform.transform);
							element.Set("rect.size", uiElem.rectTransform.size);
							element.Set("color", uiElem.color);
							element.Set("isText", uiElem.isText);

							
							if (textureData != nullptr) {
								spriteStruct.Set("offset", uiElem.spriteOffset);
								spriteStruct.Set("size", uiElem.spriteSize);
								spriteStruct.Set("texSize", textureData->GetSize());
								element.Set("sprite", spriteStruct);
							}
							element.Set("sprite.isActive", textureData != nullptr);
							FrameMarkEnd(tracy_RenderUIElementDataName);

							elementsBuffer.Set("uiElements[" + to_string(i) + "]", element);

							if (++i == maxUIElementsPerRender) {
								glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, elementsBuffer.GetSize(), elementsBuffer.GetData().data());
								glDrawArraysInstanced(GL_POINTS, 0, 1, maxUIElementsPerRender);
							}

							renderQueue.pop();

							FrameMarkEnd(tracy_RenderUIElementName);
							++elementId;
						}

						if (i != 0) {
							glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, elementsBuffer.GetSize(), elementsBuffer.GetData().data());
							glDrawArraysInstanced(GL_POINTS, 0, 1, i);
						}

						FrameMarkEnd(tracy_RenderUITextureName);
						++textureId;
					}

					FrameMarkEnd(tracy_RenderUIMaskName);
					++maskId;
				}

				FrameMarkEnd(tracy_RenderUILayerName);
			}

			FrameMarkEnd(tracy_RenderUICanvasName);
			++canvasId;
		}

		FrameMarkStart(tracy_RenderUIEnd);
		_renderQueue.clear();
		glBindBuffer(GL_UNIFORM_BUFFER, NULL);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, NULL);
		glBindVertexArray(NULL);
		FrameMarkEnd(tracy_RenderUIEnd);
	}
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