#include <GraphicEnigineManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

void GraphicEngineManager::Init()
{
	ShaderManager::Init();
	UIRenderingManager::Init();
	MeshRenderingManager::Init();

	glClearColor(.1f, .1f, .1f, 1.f);
}

void GraphicEngineManager::End()
{

	ShaderManager::UnloadAll();
	MeshRenderingManager::UnloadAll();
}

void GraphicEngineManager::Render()
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

void GraphicEngineManager::DepthRender()
{
#if DEBUG_GRAPHIC_ENGINE
	float startDepthRenderingTime = glfwGetTime();
#endif
	MeshRenderingManager::RenderDepthMap();
#if DEBUG_GRAPHIC_ENGINE
	SPDLOG_INFO("Depth Randering Time: {}", glfwGetTime() - startDepthRenderingTime);
#endif
}