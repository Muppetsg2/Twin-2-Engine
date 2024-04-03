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
	vector<Character*> characters = _font->GetText(_text, _size);
	for (auto& c : characters)
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
	_text = text;
}

void Text::SetSize(uint32_t size)
{
	_size = size;
}

void Text::SetFont(const string& fontPath)
{
	_font = FontManager::LoadFont(fontPath);
}

void Text::SetFont(Font* font)
{
	_font = font;
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