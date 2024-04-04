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
InstatiatingModel UIRenderingManager::_spritePlane = InstatiatingModel();
Shader* UIRenderingManager::_uiShader = nullptr;

void UIRenderingManager::Init() {
	_spritePlane = ModelsManager::GetPlane();
	_uiShader = ShaderManager::GetShaderProgram("origin/UI");
}

void UIRenderingManager::Render()
{
	_uiShader->use();
	glActiveTexture(GL_TEXTURE0);
	_uiShader->setInt("sprite.img", 0);
	ivec2 canvasSize = Window::GetInstance()->GetWindowSize();
	_uiShader->setFloat("canvas.width", canvasSize.x);
	_uiShader->setFloat("canvas.height", canvasSize.y);

	while (_renderQueue.size() > 0) {
		UIElement elem = _renderQueue.front();
		
		_uiShader->setBool("isText", elem.isText);

		_uiShader->setVec4("sprite.color", (float*)&elem.color);
		_uiShader->setFloat("sprite.width", (float)elem.spriteSize.x);
		_uiShader->setFloat("sprite.height", (float)elem.spriteSize.y);
		_uiShader->setFloat("sprite.x", (float)elem.spriteOffset.x);
		_uiShader->setFloat("sprite.y", (float)elem.spriteOffset.y);
		_uiShader->setFloat("sprite.texWidth", (float)elem.textureSize.x);
		_uiShader->setFloat("sprite.texHeight", (float)elem.textureSize.y);

		glBindTexture(GL_TEXTURE_2D, elem.textureID);

		_uiShader->setFloat("canvas.elemWidth", elem.elemSize.x);
		_uiShader->setFloat("canvas.elemHeight", elem.elemSize.y);

		mat4 normalModel = mat4(mat3(transpose(inverse(elem.transform))));
		_uiShader->setMat4("model", elem.transform);
		_uiShader->setMat4("normalModel", normalModel);

		_spritePlane.GetMesh(0)->Draw(_uiShader, 1);

		_renderQueue.pop();
	}
}

void UIRenderingManager::Render(UIElement elem)
{
	_renderQueue.push(elem);
}