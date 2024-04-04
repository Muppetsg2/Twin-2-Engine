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

void Text::Render()
{
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
		if (_size != 0 && _font != nullptr) {
			// Zastêpowanie
			for (size_t i = 0; i < _text.size() && i < text.size(); ++i) {
				if (_text[i] != text[i]) {
					_textCache[i] = _font->GetCharacter(text[i], _size);
				}
			}

			// Dodawanie kolejnych
			if (_text.size() < text.size()) {
				vector<Character*> chars = _font->GetText(text.substr(_text.size(), text.size() - _text.size()), _size);
				for (auto& c : chars) _textCache.push_back(c);
			}
			// Odejmowanie nadmiaru
			else if (_text.size() > text.size()) {
				for (size_t i = 0; i < _text.size() - text.size(); ++i) _textCache.erase(_textCache.begin() + text.size() + i);
			}
		}
		else {
			_textCache.clear();
		}
		_text = text;
	}
}

void Text::SetSize(uint32_t size)
{
	if (_size != size) {
		_size = size;
		_textCache.clear();
		if (_font != nullptr) {
			_textCache = _font->GetText(_text, size);
		}
	}
}

void Text::SetFont(const string& fontPath)
{
	SetFont(FontManager::LoadFont(fontPath));
}

void Text::SetFont(Font* font)
{
	if (_font != font) {
		_font = font;
		_textCache.clear();
		if (_size != 0) {
			_textCache = _font->GetText(_text, _size);
		}
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

Font* Text::GetFont() const
{
	return _font;
}