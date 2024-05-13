#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/FontManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/LightingController.h>
#include <graphic/Window.h>

#define DEBUG_GRAPHIC_ENGINE false

namespace Twin2Engine::Graphic
{
#if _DEBUG
	static void glfw_error_callback(int error, const char* description);
	static void GLAPIENTRY ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

#endif

	class GraphicEngine
	{
	public:
		static void Init(const std::string& window_name, int32_t window_width, int32_t window_height, bool fullscreen, int32_t gl_version_major, int32_t gl_version_minor);
		static void End();

		static void UpdateBeforeRendering();
		static void Render();
		static void DepthRender();
	};
}

#endif