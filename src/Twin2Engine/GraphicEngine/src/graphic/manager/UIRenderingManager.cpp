#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/Shader.h>
#include <graphic/Window.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Tools;
using namespace std;
using namespace glm;

// STD140 OFFSETS
STD140Offsets UIRenderingManager::RectTransformOffsets{
	STD140Variable<mat4>("transform"),
	STD140Variable<vec2>("size")
};

STD140Offsets UIRenderingManager::SpriteOffsets{
	STD140Variable<uvec2>("offset"),
	STD140Variable<uvec2>("size"),
	STD140Variable<bool>("isActive")
};

STD140Offsets UIRenderingManager::TextureOffsets{
	STD140Variable<uvec2>("size"),
	STD140Variable<bool>("isActive")
};

STD140Offsets UIRenderingManager::UIElementOffsets{
	STD140Variable<STD140Offsets>("rect", UIRenderingManager::RectTransformOffsets),
	STD140Variable<STD140Offsets>("sprite", UIRenderingManager::SpriteOffsets),
	STD140Variable<vec4>("color"),
	STD140Variable<bool>("isText")
};

// STD140 STRUCTS
STD140Struct UIRenderingManager::CanvasStruct{
	STD140Variable<STD140Offsets>("canvasRect", UIRenderingManager::RectTransformOffsets),
	STD140Variable<bool>("canvasIsInWorldSpace"),
	STD140Variable<bool>("canvasIsActive")
};

STD140Struct UIRenderingManager::MaskStruct{
	STD140Variable<STD140Offsets>("maskRect", UIRenderingManager::RectTransformOffsets),
	STD140Variable<STD140Offsets>("maskSprite", UIRenderingManager::SpriteOffsets),
	STD140Variable<uvec2>("maskTexureSize"),
	STD140Variable<bool>("maskIsActive")
};

STD140Struct UIRenderingManager::UIElementsBufferStruct{
	STD140Variable<STD140Offsets>("uiElements", UIRenderingManager::UIElementOffsets, UIRenderingManager::maxUIElementsPerRender),
	STD140Variable<STD140Offsets>("elementTexture", UIRenderingManager::TextureOffsets)
};

// QUEUE
unordered_map<CanvasData*, map<int32_t, unordered_map<MaskData*, unordered_map<Texture2D*, queue<UIRenderingManager::UIElementQueueData>>>>> UIRenderingManager::_renderQueue;

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

// FORMAT
const char* const UIRenderingManager::_uiBufforElemFormat = "uiElements[{0}]";

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
	ZoneScoped;

	static const char* const tracy_RenderUIShader = "Render UI Shader";
	static const char* const tracy_RenderUICanvasName = "Render UI Canvas";
	static const char* const tracy_RenderUICanvasUBOName = "Render UI Canvas UBO";
	static const char* const tracy_RenderUILayerName = "Render UI Layer";
	static const char* const tracy_RenderUIMaskName = "Render UI Mask";
	static const char* const tracy_RenderUIMaskUBOName = "Render UI Mask UBO";
	static const char* const tracy_RenderUITextureName = "Render UI Texture";
	static const char* const tracy_RenderUIElementName = "Render UI Element";
	static const char* const tracy_RenderUIElementMaskCheck = "Render UI Element Mask Check";
	static const char* const tracy_RenderUIElementDataName = "Render UI Element Data";
	static const char* const tracy_RenderUIEnd = "Render UI End";
#endif

	if (_uiShader != nullptr) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _elemsSSBO);

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
		for (const auto& canvas : _renderQueue) {
#if TRACY_PROFILER
			FrameMarkStart(tracy_RenderUICanvasName);
#endif
			
			CanvasData* canvasData = canvas.first;

#if TRACY_PROFILER
			FrameMarkStart(tracy_RenderUICanvasUBOName);
#endif

			glBindBuffer(GL_UNIFORM_BUFFER, _canvasUBO);
			if (canvasData != nullptr) {
				CanvasStruct.Set("canvasRect.transform", canvasData->rectTransform.transform);
				CanvasStruct.Set("canvasRect.size", canvasData->rectTransform.size);
				CanvasStruct.Set("canvasIsInWorldSpace", canvasData->worldSpaceCanvas);
			}
			CanvasStruct.Set("canvasIsActive", canvasData != nullptr);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, CanvasStruct.GetSize(), CanvasStruct.GetData().data());
			
#if TRACY_PROFILER
			FrameMarkEnd(tracy_RenderUICanvasUBOName);
#endif

			for (const auto& layer : canvas.second) {
#if TRACY_PROFILER
				FrameMarkStart(tracy_RenderUILayerName);
#endif

				for (const auto& mask : layer.second) {
#if TRACY_PROFILER
					FrameMarkStart(tracy_RenderUIMaskName);
#endif

					MaskData* maskData = mask.first;
					mat4 invMaskTransform = mat4(1.f);

#if TRACY_PROFILER
					FrameMarkStart(tracy_RenderUIMaskUBOName);
#endif

					glBindBuffer(GL_UNIFORM_BUFFER, _maskUBO);
					if (maskData != nullptr) {
						invMaskTransform = inverse(maskData->rectTransform.transform);

						MaskStruct.Set("maskRect.transform", maskData->rectTransform.transform);
						MaskStruct.Set("maskRect.size", maskData->rectTransform.size);
						
						if (maskData->maskSprite != nullptr) {
							MaskStruct.Set("maskSprite.offset", maskData->maskSprite->GetOffset());
							MaskStruct.Set("maskSprite.size", maskData->maskSprite->GetSize());
							MaskStruct.Set("maskTextureSize", maskData->maskSprite->GetTexture()->GetSize());

							maskData->maskSprite->Use(1);
						}
						MaskStruct.Set("maskSprite.isActive", maskData->maskSprite != nullptr);
					}
					MaskStruct.Set("maskIsActive", maskData != nullptr);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, MaskStruct.GetSize(), MaskStruct.GetData().data());

#if TRACY_PROFILER
					FrameMarkEnd(tracy_RenderUIMaskUBOName);
#endif

					for (const auto& texture : mask.second) {
#if TRACY_PROFILER
						FrameMarkStart(tracy_RenderUITextureName);
#endif

						Texture2D* textureData = texture.first;

						if (textureData != nullptr) {
							textureData->Use(0);
							UIElementsBufferStruct.Set("elementTexture.size", textureData->GetSize());
						}
						UIElementsBufferStruct.Set("elementTexture.isActive", textureData != nullptr);

						queue<UIElementQueueData> renderQueue = texture.second;
						size_t i = 0;
						while (renderQueue.size() > 0) {
#if TRACY_PROFILER
							FrameMarkStart(tracy_RenderUIElementName);
#endif

							const UIElementQueueData& uiElem = renderQueue.front();

#if TRACY_PROFILER
							FrameMarkStart(tracy_RenderUIElementMaskCheck);
#endif

							// ADD MASK CHECKING
							if (maskData != nullptr) {
								vec4 planePoint1 = vec4(-uiElem.rectTransform.size.x * .5f, uiElem.rectTransform.size.y * .5f, 0.f, 1.f);
								vec4 planePoint2 = vec4(uiElem.rectTransform.size.x * .5f, uiElem.rectTransform.size.y * .5f, 0.f, 1.f);
								vec4 planePoint3 = vec4(uiElem.rectTransform.size.x * .5f, -uiElem.rectTransform.size.y * .5f, 0.f, 1.f);
								vec4 planePoint4 = vec4(-uiElem.rectTransform.size.x * .5f, -uiElem.rectTransform.size.y * .5f, 0.f, 1.f);

								planePoint1 = uiElem.rectTransform.transform * planePoint1;
								planePoint2 = uiElem.rectTransform.transform * planePoint2;
								planePoint3 = uiElem.rectTransform.transform * planePoint3;
								planePoint4 = uiElem.rectTransform.transform * planePoint4;

								vec2 maxPoint{};
								maxPoint.x = glm::max(glm::max(glm::max(planePoint1.x, planePoint2.x), planePoint3.x), planePoint4.x);
								maxPoint.y = glm::max(glm::max(glm::max(planePoint1.y, planePoint2.y), planePoint3.y), planePoint4.y);
								vec2 minPoint{};
								minPoint.x = glm::min(glm::min(glm::min(planePoint1.x, planePoint2.x), planePoint3.x), planePoint4.x);
								minPoint.y = glm::min(glm::min(glm::min(planePoint1.y, planePoint2.y), planePoint3.y), planePoint4.y);

								vec2 maxPointInMask = invMaskTransform * vec4(maxPoint, 0.f, 1.f);
								vec2 minPointInMask = invMaskTransform * vec4(minPoint, 0.f, 1.f);

								if (minPointInMask.x < -maskData->rectTransform.size.x * .5f || minPointInMask.y < -maskData->rectTransform.size.y * .5f || maxPointInMask.x > maskData->rectTransform.size.x * .5f || maxPointInMask.y > maskData->rectTransform.size.y * .5f) {
									renderQueue.pop();
									continue;
								}
							}

#if TRACY_PROFILER
							FrameMarkEnd(tracy_RenderUIElementMaskCheck);

							FrameMarkStart(tracy_RenderUIElementDataName);
#endif

							string elemName = vformat(_uiBufforElemFormat, make_format_args(i));

							UIElementsBufferStruct.Set(move(concat(elemName, ".rect.transform")), uiElem.rectTransform.transform);
							UIElementsBufferStruct.Set(move(concat(elemName, ".rect.size")), uiElem.rectTransform.size);
							UIElementsBufferStruct.Set(move(concat(elemName, ".color")), uiElem.color);
							UIElementsBufferStruct.Set(move(concat(elemName, ".isText")), uiElem.isText);
							
							if (textureData != nullptr) {
								if (uiElem.sprite != nullptr) {
									UIElementsBufferStruct.Set(move(concat(elemName, ".sprite.offset")), uiElem.sprite->GetOffset());
									UIElementsBufferStruct.Set(move(concat(elemName, ".sprite.size")), uiElem.sprite->GetSize());
								}
								UIElementsBufferStruct.Set(move(concat(elemName, ".sprite.isActive")), uiElem.sprite != nullptr);
							}
#if TRACY_PROFILER
							FrameMarkEnd(tracy_RenderUIElementDataName);
#endif

							if (++i == maxUIElementsPerRender) {
								glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, UIElementsBufferStruct.GetSize(), UIElementsBufferStruct.GetData().data());
								glDrawArraysInstanced(GL_POINTS, 0, 1, maxUIElementsPerRender);
								i = 0;
							}

							renderQueue.pop();

#if TRACY_PROFILER
							FrameMarkEnd(tracy_RenderUIElementName);
#endif
						}

						if (i != 0) {
							glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, UIElementsBufferStruct.GetSize(), UIElementsBufferStruct.GetData().data());
							glDrawArraysInstanced(GL_POINTS, 0, 1, i);
						}

#if TRACY_PROFILER
						FrameMarkEnd(tracy_RenderUITextureName);
#endif
					}

#if TRACY_PROFILER
					FrameMarkEnd(tracy_RenderUIMaskName);
#endif
				}

#if TRACY_PROFILER
				FrameMarkEnd(tracy_RenderUILayerName);
#endif
			}

#if TRACY_PROFILER
			FrameMarkEnd(tracy_RenderUICanvasName);
#endif
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
}

void UIRenderingManager::Render(UITextData text) 
{
	if (text.color.a == 0.f) return;

	_renderQueue[text.canvas][text.layer][text.mask][text.charTexture].push(UIElementQueueData{
		.rectTransform = text.rectTransform,
		.sprite = nullptr,
		.color = text.color,
		.isText = true
	});
}

void UIRenderingManager::Render(UIImageData image)
{
	if (image.color.a == 0.f) return;

	_renderQueue[image.canvas][image.layer][image.mask][image.sprite->GetTexture()].push(UIElementQueueData{
		.rectTransform = image.rectTransform,
		.sprite = image.sprite,
		.color = image.color,
		.isText = false
	});
}