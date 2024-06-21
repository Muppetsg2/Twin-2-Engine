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

STD140Offsets UIRenderingManager::FillDataOffsets{
	STD140Variable<uint>("type"),
	STD140Variable<uint>("subType"),
	STD140Variable<float>("offset"),
	STD140Variable<float>("progress"),
	STD140Variable<float>("rotation"),
	STD140Variable<bool>("isActive")
};

STD140Offsets UIRenderingManager::UIElementOffsets{
	STD140Variable<STD140Offsets>("rect", UIRenderingManager::RectTransformOffsets),
	STD140Variable<STD140Offsets>("sprite", UIRenderingManager::SpriteOffsets),
	STD140Variable<STD140Offsets>("fill", UIRenderingManager::FillDataOffsets),
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
	STD140Variable<STD140Offsets>("maskFill", UIRenderingManager::FillDataOffsets),
	STD140Variable<uvec2>("maskTexureSize"),
	STD140Variable<bool>("maskIsActive")
};

STD140Struct UIRenderingManager::UIElementsBufferStruct{
	STD140Variable<STD140Offsets>("uiElements", UIRenderingManager::UIElementOffsets, UIRenderingManager::maxUIElementsPerRender),
	STD140Variable<STD140Offsets>("elementTexture", UIRenderingManager::TextureOffsets)
};

// QUEUE
map<int32_t,
	unordered_map<CanvasData*,
	map<int32_t,
	unordered_map<MaskData*,
	unordered_map<Texture2D*,
	queue<UIRenderingManager::UIElementQueueData>>>>>> UIRenderingManager::_worldSpaceRenderQueue;

map<int32_t, 
	unordered_map<CanvasData*,
	map<int32_t,
	unordered_map<MaskData*,
	unordered_map<Texture2D*,
	queue<UIRenderingManager::UIElementQueueData>>>>>> UIRenderingManager::_screenSpaceRenderQueue;

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
	_worldSpaceRenderQueue.clear();
	_screenSpaceRenderQueue.clear();
	_uiShader = nullptr;

	glDeleteBuffers(1, &_elemsSSBO);
	glDeleteBuffers(1, &_maskUBO);
	glDeleteBuffers(1, &_canvasUBO);
	glDeleteBuffers(1, &_pointVBO);
	glDeleteVertexArrays(1, &_pointVAO);
}

void UIRenderingManager::RenderWorldSpace()
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	if (_uiShader == nullptr) return;
	if (_worldSpaceRenderQueue.empty()) return;

	CanvasStruct.Set("canvasIsInWorldSpace", true);
	RenderUI(_worldSpaceRenderQueue);
}

void UIRenderingManager::RenderScreenSpace()
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	if (_uiShader == nullptr) return;
	if (_screenSpaceRenderQueue.empty()) return;

	CanvasStruct.Set("canvasIsInWorldSpace", false);
	RenderUI(_screenSpaceRenderQueue);
}

void UIRenderingManager::RenderUI(map<int32_t, unordered_map<CanvasData*, map<int32_t, unordered_map<MaskData*, unordered_map<Texture2D*, queue<UIElementQueueData>>>>>>& renderQueue)
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

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _elemsSSBO);

	_uiShader->Use();
	_uiShader->SetInt("image", 0);
	_uiShader->SetInt("maskImage", 1);

	glBindVertexArray(_pointVAO);

	for (const auto& canvasLayer : renderQueue) {
		for (const auto& canvas : canvasLayer.second) {
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

						if (maskData->fill.isActive) {
							MaskStruct.Set("maskFill.type", (uint)maskData->fill.type);
							MaskStruct.Set("maskFill.subType", (uint)maskData->fill.subType);
							MaskStruct.Set("maskFill.offset", maskData->fill.offset);
							MaskStruct.Set("maskFill.progress", maskData->fill.progress);
							MaskStruct.Set("maskFill.rotation", maskData->fill.rotation);
						}
						MaskStruct.Set("maskFill.isActive", maskData->fill.isActive);
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

							// MASK CHECK
							if (maskData != nullptr) {
								// MASK FILL CHECK
								if (maskData->fill.isActive) {
									if (maskData->fill.progress - maskData->fill.offset == 0.f) {
										renderQueue.pop();
										continue;
									}
								}

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
#endif

							// UI ELEM FILL CHECK
							if (uiElem.fill.isActive) {
								if (uiElem.fill.progress - uiElem.fill.offset == 0.f) {
									renderQueue.pop();
									continue;
								}
							}

#if TRACY_PROFILER
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

							if (uiElem.fill.isActive) {
								UIElementsBufferStruct.Set(move(concat(elemName, ".fill.type")), (uint)uiElem.fill.type);
								UIElementsBufferStruct.Set(move(concat(elemName, ".fill.subType")), (uint)uiElem.fill.subType);
								UIElementsBufferStruct.Set(move(concat(elemName, ".fill.offset")), uiElem.fill.offset);
								UIElementsBufferStruct.Set(move(concat(elemName, ".fill.progress")), uiElem.fill.progress);
								UIElementsBufferStruct.Set(move(concat(elemName, ".fill.rotation")), uiElem.fill.rotation);
							}
							UIElementsBufferStruct.Set(move(concat(elemName, ".fill.isActive")), uiElem.fill.isActive);
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
							string lastElemName = vformat(_uiBufforElemFormat, make_format_args(i));
							glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, UIElementsBufferStruct.GetOffset(lastElemName), UIElementsBufferStruct.GetData().data());
							size_t textureOffset = UIElementsBufferStruct.GetOffset("elementTexture");
							glBufferSubData(GL_SHADER_STORAGE_BUFFER, textureOffset, TextureOffsets.GetSize(), UIElementsBufferStruct.GetData().data() + textureOffset);
							//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, UIElementsBufferStruct.GetSize(), UIElementsBufferStruct.GetData().data());
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
	}


#if TRACY_PROFILER
	FrameMarkStart(tracy_RenderUIEnd);
#endif

	renderQueue.clear();

	glBindBuffer(GL_UNIFORM_BUFFER, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, NULL);
	glBindVertexArray(NULL);

#if TRACY_PROFILER
	FrameMarkEnd(tracy_RenderUIEnd);
#endif
}

void UIRenderingManager::Render(UITextData text) 
{
	if (text.color.a == 0.f) return;

	UIElementQueueData elem = UIElementQueueData{
		.rectTransform = text.rectTransform,
		.fill = { 0, 0, 0.f, 0.f, 0.f, false },
		.sprite = nullptr,
		.color = text.color,
		.isText = true
	};

	if (text.canvas != nullptr) {
		if (text.canvas->worldSpaceCanvas) {
			_worldSpaceRenderQueue[text.canvas->layer][text.canvas][text.layer][text.mask][text.charTexture].push(elem);
			return;
		}

		_screenSpaceRenderQueue[text.canvas->layer][text.canvas][text.layer][text.mask][text.charTexture].push(elem);
		return;
	}
	_screenSpaceRenderQueue[0][text.canvas][text.layer][text.mask][text.charTexture].push(elem);
}

void UIRenderingManager::Render(UIImageData image)
{
	if (image.color.a == 0.f) return;

	Texture2D* texture = nullptr;
	if (image.sprite != nullptr) texture = image.sprite->GetTexture();

	UIElementQueueData elem = UIElementQueueData{
		.rectTransform = image.rectTransform,
		.fill = image.fill,
		.sprite = image.sprite,
		.color = image.color,
		.isText = false
	};

	if (image.canvas != nullptr) {
		if (image.canvas->worldSpaceCanvas) {
			_worldSpaceRenderQueue[image.canvas->layer][image.canvas][image.layer][image.mask][texture].push(elem);
			return;
		}

		_screenSpaceRenderQueue[image.canvas->layer][image.canvas][image.layer][image.mask][texture].push(elem);
		return;
	}
	_screenSpaceRenderQueue[0][image.canvas][image.layer][image.mask][texture].push(elem);
}

void UIRenderingManager::Render(UIElementData data, FillData fill, Texture2D* texture)
{
	if (data.color.a == 0.f) return;

	UIElementQueueData elem = UIElementQueueData{
		.rectTransform = data.rectTransform,
		.fill = fill,
		.sprite = nullptr,
		.color = data.color,
		.isText = false
	};

	if (data.canvas != nullptr) {
		if (data.canvas->worldSpaceCanvas) {
			_worldSpaceRenderQueue[data.canvas->layer][data.canvas][data.layer][data.mask][texture].push(elem);
			return;
		}

		_screenSpaceRenderQueue[data.canvas->layer][data.canvas][data.layer][data.mask][texture].push(elem);
		return;
	}
	_screenSpaceRenderQueue[0][data.canvas][data.layer][data.mask][texture].push(elem);
}