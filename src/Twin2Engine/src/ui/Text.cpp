#include <ui/Text.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>
#include <manager/SceneManager.h>
#include <graphic/Shader.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/FontManager.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace Graphic;
using namespace Manager;
using namespace glm;
using namespace std;

void Text::UpdateTextCache()
{
	Font* font = FontManager::GetFont(_fontId);
	if (_size != 0 && font != nullptr) {
		if (_justResizeCache) {
			// Zastêpowanie
			for (size_t i = 0; i < _oldText.size() && i < _text.size() && i < _textCache.size(); ++i) {
				if (_oldText[i] != _text[i]) {
					_totalTextWidth -= _textCache[i]->Advance >> 6;
					_textCache[i] = font->GetCharacter(_text[i], _size);
					_totalTextWidth += _textCache[i]->Advance >> 6;

					if (_textCache[i]->Size.y > _maxTextHeight) {
						_maxTextHeight = _textCache[i]->Size.y;
					}
				}
			}

			// Dodawanie kolejnych
			if (_oldText.size() < _text.size()) {
				vector<Character*> chars = font->GetText(_text.substr(_oldText.size(), _text.size() - _oldText.size() + 1), _size);
				for (auto& c : chars) {
					_textCache.push_back(c);
					_totalTextWidth += c->Advance >> 6;

					if (c->Size.y > _maxTextHeight) {
						_maxTextHeight = c->Size.y;
					}
				}
			}
			// Odejmowanie nadmiaru
			else if (_oldText.size() > _text.size()) {
				bool checkMaxSize = false;
				for (size_t i = _oldText.size(); i > _text.size(); --i) {
					_totalTextWidth -= (*(_textCache.end() - 1))->Advance >> 6;
					if ((*(_textCache.end() - 1))->Size.y == _maxTextHeight) {
						checkMaxSize = true;
					}

					_textCache.erase(_textCache.end() - 1);
				}

				// CHECK NEW MAX SIZE
				if (checkMaxSize) {
					_maxTextHeight = 0.f;
					for (auto& c : _textCache) {
						if (c->Size.y > _maxTextHeight) {
							_maxTextHeight = c->Size.y;
						}
					}
				}
			}
		}
		else {
			_totalTextWidth = 0.f;
			_maxTextHeight = 0.f;
			_textCache = font->GetText(_text, _size);
			for (auto& c : _textCache) {
				_totalTextWidth += c->Advance >> 6;

				if (c->Size.y > _maxTextHeight) {
					_maxTextHeight = c->Size.y;
				}
			}
		}
	}
	else {
		_totalTextWidth = 0.f;
		_maxTextHeight = 0.f;
		_textCache.clear();
	}
	_textCacheDirty = false;
	_justResizeCache = false;
}

void Text::Render()
{
	if (_textCacheDirty) UpdateTextCache();

	UIElement elem{};
	elem.hasTexture = true;
	elem.isText = true;
	elem.color = _color;
	elem.spriteOffset = { 0, 0 };

	mat4 model = GetTransform()->GetTransformMatrix();

	// iterate through all characters
	float x = 0.f;
	float y = 0.f;
	for (auto& c : _textCache)
	{
		float w = c->Size.x;
		float h = c->Size.y;

		float xpos = x;
		switch (_alignX) {
		case TextAlignX::LEFT:
			xpos += w * .5f + c->Bearing.x;
			break;
		case TextAlignX::CENTER:
			xpos += (w - _totalTextWidth) * .5f + c->Bearing.x;
			break;
		case TextAlignX::RIGHT:
			xpos += w * .5f - _totalTextWidth + c->Bearing.x;
			break;
		}

		float ypos = y;
		switch (_alignY) {
		case TextAlignY::BOTTOM:
			ypos += (h - _maxTextHeight) * .5f - (h - c->Bearing.y);
			break;
		case TextAlignY::TOP:
			ypos -= (h - _maxTextHeight) * .5f - (h - c->Bearing.y);
			break;
		}

		glm::mat4 tempModel = glm::translate(model, glm::vec3(xpos, ypos, 0.f));
		elem.elemSize = { w, h };
		elem.textureSize = { w, h };
		elem.spriteSize = { w, h };
		elem.transform = tempModel;
		elem.textureID = c->TextureID;

		UIRenderingManager::Render(elem);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (c->Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
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
		if (!_textCacheDirty) {
			_textCacheDirty = true;
			_justResizeCache = true;
			_oldText = _text;
		}
		_text = text;
	}
}

void Text::SetSize(uint32_t size)
{
	if (_size != size) {
		_size = size;
		_textCacheDirty = true;
		_justResizeCache = false;
	}
}

void Text::SetFont(const string& fontPath)
{
	SetFont(hash<string>()(fontPath));
}

void Text::SetFont(size_t fontId)
{
	if (_fontId != fontId) {
		_fontId = fontId;
		_textCacheDirty = true;
		_justResizeCache = false;
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
