#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/Shader.h>
#include <graphic/Window.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Tools;
using namespace std;
using namespace glm;

queue<UIElement> UIRenderingManager::_renderQueue = queue<UIElement>();
Shader* UIRenderingManager::_uiShader = nullptr;
uint32_t UIRenderingManager::_pointVAO = 0;
uint32_t UIRenderingManager::_pointVBO = 0;
uint32_t UIRenderingManager::_canvasUBO = 0;
uint32_t UIRenderingManager::_elemUBO = 0;

void UIRenderingManager::Init() {
	_uiShader = ShaderManager::GetShaderProgram("origin/UI");
	
	// CANVAS UBO
	glGenBuffers(1, &_canvasUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _canvasUBO);
	glBufferData(GL_UNIFORM_BUFFER, CanvasOffsets.GetSize(), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, _canvasUBO);

	// UI Element UBO
	glGenBuffers(1, &_elemUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _elemUBO);
	glBufferData(GL_UNIFORM_BUFFER, UIElementOffsets.GetSize(), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, _elemUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, NULL);

	// Point Buffer
	glGenVertexArrays(1, &_pointVAO);
	glGenBuffers(1, &_pointVBO);

	glBindVertexArray(_pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _pointVBO);

	glm::vec2 point(0.f);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), &point, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void UIRenderingManager::UnloadAll() {
	while (!_renderQueue.empty()) _renderQueue.pop();
	_uiShader = nullptr;

	glDeleteBuffers(1, &_elemUBO);
	glDeleteBuffers(1, &_canvasUBO);
	glDeleteBuffers(1, &_pointVBO);
	glDeleteVertexArrays(1, &_pointVAO);
}

void UIRenderingManager::Render()
{
	if (_uiShader != nullptr) {
		_uiShader->Use();
		glActiveTexture(GL_TEXTURE0);
		_uiShader->SetInt("image", 0);

		glBindBuffer(GL_UNIFORM_BUFFER, _canvasUBO);
		STD140Struct canvasStruct(CanvasOffsets);
		canvasStruct.Set("canvasSize", Window::GetInstance()->GetContentSize());
		glBufferSubData(GL_UNIFORM_BUFFER, 0, canvasStruct.GetSize(), canvasStruct.GetData().data());

		glBindBuffer(GL_UNIFORM_BUFFER, _elemUBO);
		glBindVertexArray(_pointVAO);
		while (_renderQueue.size() > 0) {
			UIElement elem = _renderQueue.front();
			STD140Struct elemStruct = MakeUIElementStruct(elem);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, elemStruct.GetSize(), elemStruct.GetData().data());

			if (elem.hasTexture) glBindTexture(GL_TEXTURE_2D, elem.textureID);

			glDrawArrays(GL_POINTS, 0, 1);

			_renderQueue.pop();
		}
		glBindBuffer(GL_UNIFORM_BUFFER, NULL);
		glBindVertexArray(NULL);
	}
}

void UIRenderingManager::Render(UIElement elem)
{
	_renderQueue.push(elem);
}