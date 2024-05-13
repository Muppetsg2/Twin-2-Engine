#include <GraphicEnigineManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

void GraphicEngineManager::Init()
{
	ShaderManager::Init();
	UIRenderingManager::Init();
	MeshRenderingManager::Init();

	glClearColor(0.f, 0.f, 0.f, 1.f);
}

void GraphicEngineManager::End()
{
	ShaderManager::UnloadAll();
	MeshRenderingManager::UnloadAll();
}

void GraphicEngineManager::UpdateBeforeRendering()
{
	MeshRenderingManager::UpdateQueues();
}

void GraphicEngineManager::Render()
{
#if DEBUG_GRAPHIC_ENGINE
	float startRenderingTime = glfwGetTime();
#endif

	MeshRenderingManager::RenderStatic();
	//MeshRenderingManager::Render();

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
	//MeshRenderingManager::RenderDepthMap();
	MeshRenderingManager::RenderDepthMapStatic();
#if DEBUG_GRAPHIC_ENGINE
	SPDLOG_INFO("Depth Randering Time: {}", glfwGetTime() - startDepthRenderingTime);
#endif
}