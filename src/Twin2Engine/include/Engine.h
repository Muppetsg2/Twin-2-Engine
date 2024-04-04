#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <imgui_impl/imgui_impl_glfw.h>
#include <imgui_impl/imgui_impl_opengl3.h>

#include <stb_image.h>

// Soloud
#include <soloud.h>
#include <soloud_wav.h>

// HID
#include <core/Input.h>

// TIME
#include <core/Time.h>

// WINDOW
#include <graphic/Window.h>

#include <core/EventHandler.h>

// MANAGERS
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/FontManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <manager/AudioManager.h>

// GAME OBJECT
#include <core/GameObject.h>
#include <core/MeshRenderer.h>
#include <ui/Image.h>
#include <ui/Text.h>

// AUDIO
#include <core/AudioComponent.h>

// GRAPHIC_ENGINE
#include <GraphicEnigine.h>

//LOGGER
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// STANDARD LIBRARY
#include <memory>

// COLLISIONS
#include <CollisionManager.h>
#include <core/BoxColliderComponent.h>
#include <core/CapsuleColliderComponent.h>
#include <core/SphereColliderComponent.h>

// CAMERA
#include <core/CameraComponent.h>

namespace Twin2Engine {
	class Engine {
	private:
		GraphicEngine::Window* _mainWindow = nullptr;
		Core::GameObject _rootObject = Core::GameObject();
		GraphicEngine::GraphicEngine _graphicEngine = GraphicEngine::GraphicEngine();

		GLuint UBOMatrices = 0;

		// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
		const char* glsl_version = "#version 450";
		int32_t GL_VERSION_MAJOR = 4;
		int32_t GL_VERSION_MINOR = 5;

		bool Init(const std::string& windowName, int32_t windowWidth, int32_t windowHeight);
		bool Init_OpenGL(const std::string& windowName, int32_t windowWidth, int32_t windowHeight);
		bool Init_Imgui();

		void Input();
		void Update();
		void Render();

		void Imgui_Begin();
		void Imgui_Render();
		void Imgui_End();

		void End_Frame();

		void Cleanup();

		void GameLoop();

		Core::MethodEventHandler onInputEvent = Core::MethodEventHandler();
		Core::MethodEventHandler onUpdateEvent = Core::MethodEventHandler();
		Core::MethodEventHandler onImguiRenderEvent = Core::MethodEventHandler();

	public:
		Engine() = default;
		virtual ~Engine() = default;

		Core::GameObject* CreateGameObject();

		int Start(const std::string& windowName, int32_t windowWidth, int32_t windowHeight);

		GLuint GetUBO() const;

		Core::MethodEventHandler GetOnInputEvent() const;
		Core::MethodEventHandler GetOnUpdateEvent() const;
		Core::MethodEventHandler GetOnImguiRenderEvent() const;
	};
}
