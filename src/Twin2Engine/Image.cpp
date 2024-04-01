#include <ui/Image.h>
#include <ShaderManager.h>
#include <Shader.h>
#include <vector>
#include <core/Transform.h>
#include <UIRenderingManager.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace GraphicEngine;
using namespace glm;
using namespace std;

unsigned int imageVAO;
unsigned int imageVBO;
bool imageVAOInit = false;

vector<float> imageVerticies {
	-.5f, .5f, 0.f,		0.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,

	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, -.5f, 0.f,		1.f, 1.f,	0.f, 0.f, 1.f
};

void Image::Initialize()
{
	UIRenderingManager::Register(this);

	if (!imageVAOInit) {
		glGenVertexArrays(1, &imageVAO);
		glBindVertexArray(imageVAO);
		glGenBuffers(1, &imageVBO);
		glBindBuffer(GL_ARRAY_BUFFER, imageVBO);
		glBufferData(GL_ARRAY_BUFFER, imageVerticies.size() * sizeof(float), imageVerticies.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
}

void Image::Render(const Window* window)
{
	glBindVertexArray(imageVAO);
	glBindBuffer(GL_ARRAY_BUFFER, imageVBO);

	Shader* uiShader = ShaderManager::GetShaderProgram("res/CompiledShaders/origin/UI.shdr");
	uiShader->use();
	uiShader->setVec4("sprite.color", (float*)&_color);
	uiShader->setFloat("sprite.width", (float)_sprite->GetWidth());
	uiShader->setFloat("sprite.height", (float)_sprite->GetHeight());
	uiShader->setFloat("sprite.x", (float)_sprite->GetXOffset());
	uiShader->setFloat("sprite.y", (float)_sprite->GetYOffset());
	uiShader->setFloat("sprite.texWidth", (float)_sprite->GetTexture()->GetWidth());
	uiShader->setFloat("sprite.texHeight", (float)_sprite->GetTexture()->GetHeight());
	
	_sprite->Use(0);
	uiShader->setInt("sprite.img", 0);

	ivec2 canvasSize = window->GetWindowSize();
	uiShader->setFloat("canvas.width", canvasSize.x);
	uiShader->setFloat("canvas.height", canvasSize.y);
	uiShader->setFloat("canvas.elemWidth", _width);
	uiShader->setFloat("canvas.elemHeight", _height);

	mat4 model = GetTransform()->GetTransformMatrix();
	mat4 normalModel = mat4(mat3(transpose(inverse(model))));
	uiShader->setMat4("model", model);
	uiShader->setMat4("normalModel", normalModel);

	glDrawArrays(GL_TRIANGLES, 0, imageVerticies.size() / 8);
}

void Image::OnDestroy()
{
	_sprite = nullptr;
	UIRenderingManager::Unregister(this);
}

void Image::SetSprite(Sprite* sprite)
{
	_sprite = sprite;
}

void Image::SetColor(const vec4& color)
{
	_color = color;
}

void Image::SetWidth(float width)
{
	_width = width;
}

void Image::SetHeight(float height)
{
	_height = height;
}

constexpr Sprite* Image::GetSprite() const
{
	return _sprite;
}

vec4 Image::GetColor() const
{
	return _color;
}

float Image::GetWidth() const
{
	return _width;
}

float Image::GetHeight() const
{
	return _height;
}
