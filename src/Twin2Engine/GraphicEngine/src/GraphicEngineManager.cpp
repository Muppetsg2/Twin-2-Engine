#include <GraphicEnigineManager.h>
#include <LightingController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

/*
GLuint GraphicEngineManager::_depthMapFBO = 0u;
GLuint GraphicEngineManager::_depthMap = 0u;
*/

//GraphicEngineManager::GraphicEngineManager() {
void GraphicEngineManager::Init()
{
	/*
#pragma region DepthBuffer

	glGenFramebuffers(1, &_depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);

	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);

	glm::ivec2 wSize = Window::GetInstance()->GetContentSize();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, wSize.x, wSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

#pragma endregion
	*/

	ShaderManager::Init();
	UIRenderingManager::Init();
	MeshRenderingManager::Init();

	// Standard Shaders
	//ShaderManager::CreateShaderProgram("res/shaders/Basic.shpr");
	//ShaderManager::CreateShaderProgram("res/shaders/UI.shpr");

	glClearColor(.1f, .1f, .1f, 1.f);
}

//Twin2Engine::GraphicEngine::GraphicEngineManager::~GraphicEngineManager() {
void GraphicEngineManager::End()
{
	/*
	glDeleteTextures(1, &_depthMap);
	glDeleteFramebuffers(1, &_depthMapFBO);
	*/

	ShaderManager::End();
	//UIRenderingManager::End();
	MeshRenderingManager::End();
}

void GraphicEngineManager::Render()
{
	/*
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MeshRenderingManager::RenderDepthMap();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	*/

	//CameraComponent* camera = Core::CameraComponent::GetMainCamera();

	//LightingSystem::LightingController::Instance()->RenderShadowMaps();
	//ShaderManager::UpdateDirShadowMapsTab();
	
	MeshRenderingManager::Render();
	UIRenderingManager::Render();
}

void GraphicEngineManager::DepthRender()
{
	MeshRenderingManager::RenderDepthMap();
}

/*
GLuint GraphicEngineManager::GetDepthMap()
{
	return _depthMap;
}
*/