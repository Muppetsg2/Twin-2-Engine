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
	private:
		GLuint _depthMapFBO;
		GLuint _depthMap;

	public:
		GraphicEngine();
		virtual ~GraphicEngine();

		void Render();

		GLuint GetDepthMap() const;
	};
}

#endif