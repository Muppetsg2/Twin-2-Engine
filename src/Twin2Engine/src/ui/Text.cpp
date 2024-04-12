#include <ui/Text.h>
#include <graphic/manager/FontManager.h>
#include <graphic/Shader.h>
#include <vector>
#include <core/Transform.h>
#include <graphic/manager/UIRenderingManager.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace GraphicEngine;
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
					_textCache[i] = font->GetCharacter(_text[i], _size);
				}
			}

			// Dodawanie kolejnych
			if (_oldText.size() < _text.size()) {
				vector<Character*> chars = font->GetText(_text.substr(_oldText.size(), _text.size() - _oldText.size() + 1), _size);
				for (auto& c : chars) _textCache.push_back(c);
			}
			// Odejmowanie nadmiaru
			else if (_oldText.size() > _text.size()) {
				for (size_t i = _oldText.size(); i >= _text.size(); --i) _textCache.erase(_textCache.end() - 1);
			}
		}
		else {
			_textCache = font->GetText(_text, _size);
		}
	}
	else {
		_textCache.clear();
	}
	_textCacheDirty = false;
	_justResizeCache = false;
}

void Text::Render()
{
	if (_textCacheDirty) UpdateTextCache();

	UIElement elem{};
	elem.isText = true;
	elem.color = _color;
	elem.spriteOffset = { 0, 0 };

	mat4 model = GetTransform()->GetTransformMatrix();

	// iterate through all characters
	float x = 0.f;
	float y = 0.f;
	float scale = 1.f;
	for (auto& c : _textCache)
	{
		float xpos = x + c->Bearing.x * scale;
		float ypos = y - (c->Size.y - c->Bearing.y) * scale;

		float w = c->Size.x * scale;
		float h = c->Size.y * scale;

		glm::mat4 tempModel = glm::translate(model, glm::vec3(xpos + w / 2.f, ypos + h / 2.f, 0.f));
		elem.elemSize = { w, h };
		elem.textureSize = { w, h };
		elem.spriteSize = { w, h };
		elem.transform = tempModel;
		elem.textureID = c->TextureID;

		UIRenderingManager::Render(elem);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (c->Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

void Text::SetColor(const vec4& color)
{
	_color = color;
}

void Text::SetText(const string& text)
{
	if (_text != text) {
		_oldText = _text;
		_text = text;

		if (!_textCacheDirty) {
			_textCacheDirty = true;
			_justResizeCache = true;
		}
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