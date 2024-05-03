#include <GraphicEnigine.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

void GraphicEngine::Init()
{
	ShaderManager::Init();
	UIRenderingManager::Init();
	MeshRenderingManager::Init();

	glClearColor(.1f, .1f, .1f, 1.f);
}

void GraphicEngine::End()
{
	ShaderManager::UnloadAll();
	MeshRenderingManager::UnloadAll();
}

void GraphicEngine::Render()
{
#if DEBUG_GRAPHIC_ENGINE
	float startRenderingTime = glfwGetTime();
#endif
	MeshRenderingManager::Render();
#if DEBUG_GRAPHIC_ENGINE
	SPDLOG_INFO("Randering Time: {}", glfwGetTime() - startRenderingTime);
	float startUIRenderingTime = glfwGetTime();
#endif
	UIRenderingManager::Render();
#if DEBUG_GRAPHIC_ENGINE
	SPDLOG_INFO("UIRandering Time: {}", glfwGetTime() - startUIRenderingTime);
#endif
}

void GraphicEngine::DepthRender()
{
#if DEBUG_GRAPHIC_ENGINE
	float startDepthRenderingTime = glfwGetTime();
#endif
	MeshRenderingManager::RenderDepthMap();
#if DEBUG_GRAPHIC_ENGINE
	SPDLOG_INFO("Depth Randering Time: {}", glfwGetTime() - startDepthRenderingTime);
#endif
}