﻿#include <GraphicEnigine.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

#if _DEBUG
void Twin2Engine::Graphic::glfw_error_callback(int error, const char* description)
{
	spdlog::error("Glfw Error {0}: {1}\n", error, description);
}

void GLAPIENTRY Twin2Engine::Graphic::ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	//if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return; // Chce ignorowac notyfikacje

	std::string severityS = "";
	if (severity == GL_DEBUG_SEVERITY_HIGH) severityS = "HIGHT";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM) severityS = "MEDIUM";
	else if (severity == GL_DEBUG_SEVERITY_LOW) severityS = "LOW";
	else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) severityS = "NOTIFICATION";

	if (type == GL_DEBUG_TYPE_ERROR) {
		spdlog::error("GL CALLBACK: type = ERROR, severity = {0}, message = {1}\n", severityS, message);
	}
	else if (type == GL_DEBUG_TYPE_MARKER) {
		spdlog::info("GL CALLBACK: type = MARKER, severity = {0}, message = {1}\n", severityS, message);
	}
	else {
		std::string typeS = "";
		if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) typeS = "DEPRACTED BEHAVIOUR";
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) typeS = "UNDEFINED BEHAVIOUR";
		else if (type == GL_DEBUG_TYPE_PORTABILITY) typeS = "PORTABILITY";
		else if (type == GL_DEBUG_TYPE_PERFORMANCE) typeS = "PERFORMANCE";
		else if (type == GL_DEBUG_TYPE_PUSH_GROUP) typeS = "PUSH GROUP";
		else if (type == GL_DEBUG_TYPE_POP_GROUP) typeS = "POP GROUP";
		else if (type == GL_DEBUG_TYPE_OTHER) typeS = "OTHER";
		spdlog::warn("GL CALLBACK: type = {0}, severity = {1}, message = {2}\n", typeS, severityS, message);
	}
}
#endif

void GraphicEngine::Init(const std::string& window_name, int32_t window_width, int32_t window_height, bool fullscreen, int32_t gl_version_major, int32_t gl_version_minor)
{
	// Setup window
#if _DEBUG
	glfwSetErrorCallback(glfw_error_callback);
#endif

	if (!glfwInit())
	{
		spdlog::error("Failed to initalize GLFW!");
		return;
	}

	// GL 4.5 + GLSL 450
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	Window::MakeWindow(window_name, { window_width, window_height }, fullscreen);

	bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (err)
	{
		spdlog::error("Failed to initialize OpenGL loader!");
		return;
	}
	spdlog::info("Successfully initialized OpenGL loader!");


#ifdef _DEBUG
	// Debugging
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(ErrorMessageCallback, 0);

	const GLubyte* renderer = glGetString(GL_RENDERER);
	spdlog::info("Graphic Card: {0}", (char*)renderer);
#endif

	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Face Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	ShaderManager::Init();
	UIRenderingManager::Init();
	MeshRenderingManager::Init();

	glClearColor(.1f, .1f, .1f, 1.f);
}

void GraphicEngine::End()
{
	MeshRenderingManager::UnloadAll();
	UIRenderingManager::UnloadAll();
	ShaderManager::UnloadAll();
	SpriteManager::UnloadAll();
	TextureManager::UnloadAll();
	FontManager::UnloadAll();
	LightingController::UnloadAll();
	Window::FreeAll();

	glfwTerminate();
}

void GraphicEngine::UpdateBeforeRendering()
{
	MeshRenderingManager::UpdateQueues();
}

void GraphicEngine::Render()
{
#if DEBUG_GRAPHIC_ENGINE
	float startRenderingTime = glfwGetTime();
#endif
	//MeshRenderingManager::RenderStatic();
	MeshRenderingManager::RenderStatic();
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
	MeshRenderingManager::PreRender();
	//MeshRenderingManager::RenderDepthMapStatic();
#if DEBUG_GRAPHIC_ENGINE
	SPDLOG_INFO("Depth Randering Time: {}", glfwGetTime() - startDepthRenderingTime);
#endif
}