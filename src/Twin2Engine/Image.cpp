#include <ui/Image.h>
#include "../GraphicEngine/include/ShaderManager.h"
#include "../GraphicEngine/include/Shader.h"
#include <vector>
#include <core/Transform.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace GraphicEngine;
using namespace glm;
using namespace std;

unsigned int VAO;
unsigned int VBO;

vector<float> verticies{
	-.5f, .5f, 0.f,		0.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,

	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, -.5f, 0.f,		1.f, 1.f,	0.f, 0.f, 1.f
};

void Image::Inizialize()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8, (void*)3);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8, (void*)5);
	glEnableVertexAttribArray(2);
}

void Image::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Shader* uiShader = ShaderManager::GetShaderProgram("origin/UI");
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

	mat4 model = getTransform()->GetTransformMatrix();
	mat4 normalModel = mat4(mat3(transpose(inverse(model))));
	uiShader->setMat4("model", model);
	uiShader->setMat4("normalModel", normalModel);

	glDrawArrays(GL_TRIANGLES, 0, verticies.size() / 8);
}

void Image::OnDestroy()
{
	_sprite = nullptr;
}

void Image::SetSprite(Sprite* sprite)
{
	_sprite = sprite;
}

void Image::SetColor(const vec4& color)
{
	_color = color;
}

constexpr Sprite* Image::GetSprite() const
{
	return _sprite;
}

vec4 Image::GetColor() const
{
	return _color;
}