#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/MeshRenderingManager.h>

#include <graphic/manager/UIRenderingManager.h>

#include <graphic/Window.h>

namespace Twin2Engine::GraphicEngine
{
	class GraphicEngine
	{
	public:
		GraphicEngine()
		{
			Manager::ShaderManager::Init();
		}

		void Render(const Window* window)
		{
			glClearColor(.1f, .1f, .1f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Manager::MeshRenderingManager::Render();
			Manager::UIRenderingManager::Render(window);
		}
	};
}

#endif