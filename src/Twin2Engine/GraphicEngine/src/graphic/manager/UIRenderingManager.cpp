#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/Shader.h>
#include <graphic/manager/ModelsManager.h>
#include <graphic/InstatiatingModel.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;
using namespace std;
using namespace glm;

queue<UIElement> UIRenderingManager::_renderQueue = queue<UIElement>();

void UIRenderingManager::Render()
{
	InstatiatingModel spritePlane = ModelsManager::GetPlane();
	Shader* uiShader = ShaderManager::GetShaderProgram("origin/UI");
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

		ivec2 canvasSize = Window::GetInstance()->GetWindowSize();
		uiShader->setFloat("canvas.width", canvasSize.x);
		uiShader->setFloat("canvas.height", canvasSize.y);
		uiShader->setFloat("canvas.elemWidth", elem.elemSize.x);
		uiShader->setFloat("canvas.elemHeight", elem.elemSize.y);

		mat4 normalModel = mat4(mat3(transpose(inverse(elem.transform))));
		uiShader->setMat4("model", elem.transform);
		uiShader->setMat4("normalModel", normalModel);

		spritePlane.GetMesh(0)->Draw(uiShader, 1);

		_renderQueue.pop();
	}
}

void UIRenderingManager::Render(UIElement elem)
{
	_renderQueue.push(elem);
}