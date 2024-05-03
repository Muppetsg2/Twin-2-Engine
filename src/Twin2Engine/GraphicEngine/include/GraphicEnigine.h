#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/UIRenderingManager.h>

#include <graphic/Window.h>

#define DEBUG_GRAPHIC_ENGINE false

namespace Twin2Engine::Graphic
{
	class GraphicEngine
	{
	public:
		static void Init();
		static void End();

		static void Render();
		static void DepthRender();
	};
}

#endif