#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/UIRenderingManager.h>

#include <graphic/Window.h>

#include <core/CameraComponent.h>

namespace Twin2Engine::GraphicEngine
{
	class GraphicEngineManager
	{

	private:
		//static GLuint _depthMapFBO;
		//static GLuint _depthMap;

	public:
		//GraphicEngineManager();
		//virtual ~GraphicEngineManager();

		static void Init();
		static void End();


		static void Render();
		static void DepthRender();

		//static GLuint GetDepthMap();
	};
}

#endif