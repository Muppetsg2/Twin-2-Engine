#include <ui/Text.h>
#include <core/Transform.h>
#include <core/CameraComponent.h>
#include <tools/YamlConverters.h>
#include <manager/SceneManager.h>
#include <graphic/Shader.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/FontManager.h>
#include <tools/EventHandler.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace Graphic;
using namespace Manager;
using namespace Tools;
using namespace glm;
using namespace std;

void Text::UpdateTextCharCache(Font* font, const string& newText, const string& oldText, float& textWidth, float& textHeight, std::vector<TextCharacter>& textCharCache) {
	if (_size != 0 && font != nullptr) {
		const float lineHeight = _size * 1.2f;
		glm::vec2 currPos = glm::vec2(0.f);
		textWidth = 0.f;
		textHeight = lineHeight;
		textCharCache.clear();

		// TO CALCULATE TEXT WIDTH
		float lineWidth = 0.f;

		// TEXT WRAPPING INFO
		bool wasSpaceInLine = false;
		size_t spaceCharIdx = 0;
		size_t lastIdx = 0;

		// TEXT ALIGN X INFO
		size_t lineStartIdx = 0;
		float linePadding = 0.f;

		// NEW LINE FUNC
		Action<size_t> newLine = [&](size_t i) -> void {
			if (_alignX == TextAlignX::CENTER) {
				for (size_t idx = lineStartIdx; idx < i; ++idx) {
					textCharCache[idx].position.x += (_width - lineWidth) * .5f - linePadding;
				}
			}
			else if (_alignX == TextAlignX::RIGHT) {
				for (size_t idx = lineStartIdx; idx < i; ++idx) {
					textCharCache[idx].position.x += _width - lineWidth - linePadding * 2;
				}
			}

			currPos.x = 0.f;
			currPos.y -= lineHeight;
			textHeight += lineHeight;

			if (lineWidth > textWidth) {
				textWidth = lineWidth;
			}
			lineWidth = 0.f;

			if (_textWrapping) {
				wasSpaceInLine = false;
				spaceCharIdx = 0;
			}

			lineStartIdx = i;
		};
		for (size_t i = 0; i < _text.size(); ++i) {
			if (_text[i] == '\n') {
				newLine(i);
				continue;
			}

			if (_text[i] == ' ' && _textWrapping) {
				wasSpaceInLine = true;
				spaceCharIdx = i;
			}

			Character* c;
			if (_textWrapping && i < lastIdx) {
				c = textCharCache[i].character;
			}
			else {
				c = font->GetCharacter(_text[i], _size);
			}

			if (i == lineStartIdx) {
				linePadding = c->Bearing.x;
			}

			float w = c->Size.x;
			float h = c->Size.y;

			bool goodPos = false;
			glm::vec2 charPos;
			while (!goodPos) {
				charPos = currPos;
				charPos.x += (w - _width) * .5f + c->Bearing.x;

				switch (_alignY) {
				case TextAlignY::BOTTOM:
					charPos.y += h * .5f - _size * .25f - (h - c->Bearing.y);
					break;
				case TextAlignY::TOP:
					charPos.y -= h * .5f - _size * .25f - (h - c->Bearing.y);
					break;
				}

				if (_textWrapping && charPos.x + w * .5f > _width * .5f) {
					if (wasSpaceInLine) {
						lastIdx = i;
						i = spaceCharIdx;
						for (size_t idx = i; idx < lastIdx; ++idx) {
							lineWidth -= textCharCache[idx].character->Advance >> 6;
						}
						newLine(i);
						break;
					}
					else {
						newLine(i);
					}
				}
				else {
					goodPos = true;
				}
			}

			if (!goodPos) {
				continue;
			}

			if (_textWrapping && i < lastIdx) {
				textCharCache[i].position = charPos;
			}
			else {
				textCharCache.push_back({ c, charPos });
			}

			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			currPos.x += c->Advance >> 6; // bitshift by 6 to get value in pixels (2^6 = 64)
			lineWidth += c->Advance >> 6;
		}

		if (_alignX == TextAlignX::CENTER) {
			for (size_t idx = lineStartIdx; idx < textCharCache.size(); ++idx) {
				textCharCache[idx].position.x += (_width - lineWidth) * .5f - linePadding;
			}
		}
		else if (_alignX == TextAlignX::RIGHT) {
			for (size_t idx = lineStartIdx; idx < textCharCache.size(); ++idx) {
				textCharCache[idx].position.x += _width - lineWidth - linePadding * 2.f;
			}
		}

		if (lineWidth > textWidth) {
			textWidth = lineWidth;
		}
	}
	else {
		textWidth = 0.f;
		textHeight = 0.f;
		textCharCache.clear();
	}
}

void Text::UpdateTextMesh()
{
	Font* font = FontManager::GetFont(_fontId);

	// Update Text Char Cache
	UpdateTextCharCache(font, _text, _oldText, _textWidth, _textHeight, _textCharCache);

	// Update Display Text
	_displayText = _text;

	// Update Display Text Char Cache
	UpdateTextCharCache(font, _displayText, _oldDisplayText, _displayTextWidth, _displayTextHeight, _displayTextCharCache);

	/*if (_size != 0 && font != nullptr) {
		const float lineHeight = _size * 1.2f;
		_displayTextCharCache.clear();
		_displayTextWidth = 0.f;
		_displayTextHeight = lineHeight;
		for (size_t i = 0; i < _textCharCache.size(); ++i) {
			TextCharacter c = _textCharCache[i];
			if (_overflow == TextOverflow::Truncate) {
				glm::vec2 pos = c.position;
				glm::vec2 size = c.character->Size;
				if (pos.x + size.x * .5f > _width * .5f || pos.x - size.x * .5f < -_width * .5f || pos.y + size.y * .5f > _height * .5f || pos.y - size.y * .5f < -_height * .5f) {
					continue;
				}
			}
			_displayTextWidth += c.character->Advance >> 6;
			_displayTextCharCache.push_back(c);
		}
	}*/
}

void Text::Update()
{
	UpdateTextMesh();
}

void Text::Render()
{
	UIElement elem{};
	elem.hasTexture = true;
	elem.isText = true;
	elem.color = _color;
	elem.spriteOffset = { 0, 0 };
	elem.canvasSize = Window::GetInstance()->GetContentSize();
	elem.worldSpaceCanvas = false;

	mat4 model = GetTransform()->GetTransformMatrix();
	if (_overflow != TextOverflow::Overflow) {
		elem.maskTransform = model;
		elem.maskSize = { _width, _height };
		elem.useMask = true;
	}

	// iterate through all characters
	for (auto& c : _displayTextCharCache)
	{
		float w = c.character->Size.x;
		float h = c.character->Size.y;

		glm::mat4 tempModel = glm::translate(model, glm::vec3(c.position, 0.f));
		elem.elemSize = { w, h };
		elem.textureSize = { w, h };
		elem.spriteSize = { w, h };
		elem.elemTransform = tempModel;
		elem.textureID = c.character->TextureID;

		UIRenderingManager::Render(elem);
	}
}

YAML::Node Text::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<string>());
	node["type"] = "Text";
	node["text"] = _text;
	node["color"] = _color;
	node["size"] = _size;
	node["font"] = SceneManager::GetFontSaveIdx(_fontId);
	return node;
}

void Text::SetColor(const vec4& color)
{
	_color = color;
}

void Text::SetText(const string& text)
{
	if (_text != text) {
		/*if (!_textCacheDirty) {
			_textCacheDirty = true;
			_justResizeCache = true;
			_oldText = _text;
		}*/
		_textDirty = true;
		_oldText = _text;
		_text = text;
	}
}

void Text::SetSize(uint32_t size)
{
	if (_size != size) {
		_size = size;
		_textDirty = true;
		//_textCacheDirty = true;
		//_justResizeCache = false;
	}
}

void Text::SetWidth(float width)
{
	_width = width;
}

void Text::SetHeight(float height)
{
	_height = height;
}

void Text::SetFont(const string& fontPath)
{
	SetFont(hash<string>()(fontPath));
}

void Text::SetFont(size_t fontId)
{
	if (_fontId != fontId) {
		_fontId = fontId;
		_textDirty = true;
		//_textCacheDirty = true;
		//_justResizeCache = false;
	}
}

void Text::SetTextAlignX(const TextAlignX& alignX)
{
	_alignX = alignX;
}

void Text::SetTextAlignY(const TextAlignY& alignY)
{
	_alignY = alignY;
}

void Text::SetTextWrapping(bool textWrapping)
{
	_textWrapping = textWrapping;
}

void Text::SetTextOverflow(const TextOverflow& overflow)
{
	_overflow = overflow;
}

vec4 Text::GetColor() const
{
	return _color;
}

string Text::GetText() const
{
	return _text;
}

uint32_t Text::GetSize() const
{
	return _size;
}

float Text::GetHeight() const
{
	return _height;
}

float Text::GetWidth() const
{
	return _width;
}

size_t Text::GetFontId() const
{
	return _fontId;
}

Font* Text::GetFont() const
{
	return FontManager::GetFont(_fontId);
}

TextAlignX Text::GetTextAlignX() const
{
	return _alignX;
}

TextAlignY Text::GetTextAlignY() const
{
	return _alignY;
}

bool Text::IsTextWrapping() const
{
	return _textWrapping;
}

TextOverflow Text::GetTextOverflow() const
{
	return _overflow;
}