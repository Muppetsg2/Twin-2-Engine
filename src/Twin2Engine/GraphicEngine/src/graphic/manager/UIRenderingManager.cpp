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
	_uiShader->Use();
	glActiveTexture(GL_TEXTURE0);
	_uiShader->SetInt("sprite.img", 0);
	ivec2 canvasSize = Window::GetInstance()->GetWindowSize();
	_uiShader->SetFloat("canvas.width", canvasSize.x);
	_uiShader->SetFloat("canvas.height", canvasSize.y);

	while (_renderQueue.size() > 0) {
		UIElement elem = _renderQueue.front();
		
		_uiShader->SetBool("isText", elem.isText);

		_uiShader->SetVec4("sprite.color", elem.color);
		_uiShader->SetFloat("sprite.width", (float)elem.spriteSize.x);
		_uiShader->SetFloat("sprite.height", (float)elem.spriteSize.y);
		_uiShader->SetFloat("sprite.x", (float)elem.spriteOffset.x);
		_uiShader->SetFloat("sprite.y", (float)elem.spriteOffset.y);
		_uiShader->SetFloat("sprite.texWidth", (float)elem.textureSize.x);
		_uiShader->SetFloat("sprite.texHeight", (float)elem.textureSize.y);

		glBindTexture(GL_TEXTURE_2D, elem.textureID);

		_uiShader->SetFloat("canvas.elemWidth", elem.elemSize.x);
		_uiShader->SetFloat("canvas.elemHeight", elem.elemSize.y);

		mat4 normalModel = mat4(mat3(transpose(inverse(elem.transform))));
		_uiShader->SetMat4("model", elem.transform);
		_uiShader->SetMat4("normalModel", normalModel);

		_spritePlane.GetMesh(0)->Draw(_uiShader, 1);

		_renderQueue.pop();
	}
}

void UIRenderingManager::Render(UIElement elem)
{
	_renderQueue.push(elem);
}