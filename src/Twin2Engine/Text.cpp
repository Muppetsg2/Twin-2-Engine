#include <ui/Text.h>
#include <ShaderManager.h>
#include <manager/FontManager.h>
#include <Shader.h>
#include <vector>
#include <core/Transform.h>
#include <UIRenderingManager.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace GraphicEngine;
using namespace Manager;
using namespace glm;
using namespace std;

unsigned int textVAO = 0;
unsigned int textVBO = 0;
bool textVAOInit = false;

vector<float> textVerticies{
	-.5f, .5f, 0.f,		0.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,

	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, -.5f, 0.f,		1.f, 1.f,	0.f, 0.f, 1.f
};

void Text::UpdateTextTextures()
{
	if (_fontPath != "" && _size != 0) {
		_textTextures = FontManager::GetText(_fontPath, _size, _text);
	}
}

void Text::Initialize()
{
	UIRenderingManager::Register(this);

	if (!textVAOInit) {
		glGenVertexArrays(1, &textVAO);
		glBindVertexArray(textVAO);
		glGenBuffers(1, &textVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferData(GL_ARRAY_BUFFER, textVerticies.size() * sizeof(float), textVerticies.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		textVAOInit = true;
	}
}

void Text::Render(const Window* window)
{
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);

	Shader* uiShader = ShaderManager::GetShaderProgram("res/CompiledShaders/origin/Text.shdr");
	uiShader->use();
	uiShader->setVec4("text.color", (float*)&_color);

	glActiveTexture(GL_TEXTURE0);
	uiShader->setInt("text.glyph", 0);

	ivec2 canvasSize = window->GetWindowSize();
	uiShader->setFloat("canvas.width", canvasSize.x);
	uiShader->setFloat("canvas.height", canvasSize.y);

	mat4 model = GetTransform()->GetTransformMatrix();
	mat4 normalModel = mat4(mat3(transpose(inverse(model))));
	uiShader->setMat4("normalModel", normalModel);

	// iterate through all characters
	float x = 0.f;
	float y = 0.f;
	float scale = 1.f;
	for (auto& c : _textTextures)
	{
		float xpos = x + c->Bearing.x * scale;
		float ypos = y - (c->Size.y - c->Bearing.y) * scale;

		float w = c->Size.x * scale;
		float h = c->Size.y * scale;

		glm::mat4 tempModel = glm::translate(model, glm::vec3(xpos + w / 2.f, ypos + h / 2.f, 0.f));
		uiShader->setFloat("canvas.elemWidth", w);
		uiShader->setFloat("canvas.elemHeight", h);
		uiShader->setMat4("model", tempModel);

		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, c->TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, textVerticies.size() / 8);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (c->Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::OnDestroy()
{
	UIRenderingManager::Unregister(this);
}

void Text::SetColor(const vec4& color)
{
	_color = color;
}

void Text::SetText(const string& text)
{
	if (_text != text) {
		_text = text;
		UpdateTextTextures();
	}
}

void Text::SetSize(uint32_t size)
{
	if (_size != size) {
		_size = size;
		UpdateTextTextures();
	}
}

void Text::SetFontPath(const std::string& fontPath)
{
	if (_fontPath != fontPath) {
		_fontPath = fontPath;
		UpdateTextTextures();
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

string Text::GetFontPath() const
{
	return _fontPath;
}