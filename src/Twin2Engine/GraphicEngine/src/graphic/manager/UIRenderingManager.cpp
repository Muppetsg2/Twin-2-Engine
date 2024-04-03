#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/Shader.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;
using namespace std;
using namespace glm;

unsigned int spriteVAO;
unsigned int spriteVBO;
bool spriteVAOInit = false;

vector<float> spriteVerticies{
	-.5f, .5f, 0.f,		0.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,

	.5f, .5f, 0.f,		1.f, 0.f,	0.f, 0.f, 1.f,
	-.5f, -.5f, 0.f,	0.f, 1.f,	0.f, 0.f, 1.f,
	.5f, -.5f, 0.f,		1.f, 1.f,	0.f, 0.f, 1.f
};

queue<UIElement> UIRenderingManager::_renderQueue = queue<UIElement>();

void UIRenderingManager::Render(const Window* window)
{
	if (!spriteVAOInit) {
		glGenVertexArrays(1, &spriteVAO);
		glBindVertexArray(spriteVAO);
		glGenBuffers(1, &spriteVBO);
		glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
		glBufferData(GL_ARRAY_BUFFER, spriteVerticies.size() * sizeof(float), spriteVerticies.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(spriteVAO);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	Shader* uiShader = ShaderManager::GetShaderProgram("res/CompiledShaders/origin/UI.shdr");
	uiShader->use();
	glActiveTexture(GL_TEXTURE0);
	uiShader->setInt("sprite.img", 0);

	while (_renderQueue.size() > 0) {
		UIElement elem = _renderQueue.front();
		
		uiShader->setBool("isText", elem.isText);

		uiShader->setVec4("sprite.color", (float*)&elem.color);
		uiShader->setFloat("sprite.width", (float)elem.spriteSize.x);
		uiShader->setFloat("sprite.height", (float)elem.spriteSize.y);
		uiShader->setFloat("sprite.x", (float)elem.spriteOffset.x);
		uiShader->setFloat("sprite.y", (float)elem.spriteOffset.y);
		uiShader->setFloat("sprite.texWidth", (float)elem.textureSize.x);
		uiShader->setFloat("sprite.texHeight", (float)elem.textureSize.y);

		glBindTexture(GL_TEXTURE_2D, elem.textureID);

		ivec2 canvasSize = window->GetWindowSize();
		uiShader->setFloat("canvas.width", canvasSize.x);
		uiShader->setFloat("canvas.height", canvasSize.y);
		uiShader->setFloat("canvas.elemWidth", elem.elemSize.x);
		uiShader->setFloat("canvas.elemHeight", elem.elemSize.y);

		mat4 normalModel = mat4(mat3(transpose(inverse(elem.transform))));
		uiShader->setMat4("model", elem.transform);
		uiShader->setMat4("normalModel", normalModel);

		glDrawArrays(GL_TRIANGLES, 0, spriteVerticies.size() / 8);

		_renderQueue.pop();
	}

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void UIRenderingManager::Render(UIElement elem)
{
	_renderQueue.push(elem);
}