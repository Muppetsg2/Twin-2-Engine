#pragma once

#include <core/EventHandler.h>

namespace Twin2Engine::GraphicEngine {
	class Window {
	private:
		static Window* _instance;

		GLFWwindow* _window;
		glm::ivec2 _minSizeLimits = { GLFW_DONT_CARE, GLFW_DONT_CARE };
		glm::ivec2 _maxSizeLimits = { GLFW_DONT_CARE, GLFW_DONT_CARE };
		glm::ivec2 _aspectRatio = { GLFW_DONT_CARE, GLFW_DONT_CARE };
		int _refreshRate = 0;
		bool _vsyncOn = false;

		Window(const std::string& title, const glm::ivec2& size, bool fullscreen = false);

		static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	public:
		static Twin2Engine::Core::MethodEventHandler OnWindowSizeEvent;
		
		virtual ~Window();

		static Window* MakeWindow(const std::string& title, const glm::ivec2& size, bool fullscreen = false);
		static Window* GetInstance();

		static void FreeAll();

#pragma region GLOBAL
#pragma region GLOBAL_GETTERS
		std::string GetTitle() const;
		glm::ivec2 GetContentSize() const;
		glm::ivec2 GetWindowSize() const;
		GLFWmonitor* GetMonitor() const;
		GLFWwindow* GetWindow() const;
		glm::ivec2 GetAspectRatio() const;
		bool IsFullscreen() const;
		bool IsWindowed() const;
		bool IsClosed() const;
		bool IsMinimized() const;
		bool IsHidden() const;
		bool IsFocused() const;
		bool IsHovered() const;
		bool IsVisible() const;
		bool IsTransparent() const;
		bool IsVSyncOn() const;
		bool IsMousePassThrough() const;
#pragma endregion
#pragma region GLOBAL_SETTERS
		void SetTitle(const std::string& title);
		void SetWindowSize(const glm::ivec2& size);
		void SetFullscreen(GLFWmonitor* monitor, const glm::ivec2& size, int refreshRate = 60);
		void SetFullscreen(GLFWmonitor* monitor);
		void SetWindowed(const glm::ivec2& pos, const glm::ivec2& size);
		void SetWindowed();
		void SetOpacity(float opacity);
		void SetWindowIcons(const std::vector<GLFWimage>& icons);
		void ResetWindowIcons();
		void EnableTransparency(bool enabled = true);
		void EnableVSync(bool enabled = true);
		void EnableMousePassThrough(bool enabled = true);
		void Minimize();
		void Restore();
		void Hide();
		void Show();
		void Focus();
		void Close();
		void RequestAttention();
#pragma endregion
#pragma endregion

#pragma region WINDOWED
#pragma region WINDOWED_GETTERS
		glm::ivec2 GetWindowPos() const;
		glm::ivec2 GetMaxSizeLimits() const;
		glm::ivec2 GetMinSizeLimits() const;
		float GetOpacity() const;
		bool IsMaximized() const;
		bool IsResizable() const;
		bool IsDecorated() const;
		bool IsFloating() const;
#pragma endregion
#pragma region WINDOWED_SETTERS
		void SetWindowPos(const glm::ivec2& pos);
		void SetAspectRatio(const glm::ivec2& ratio);
		void ResetAspectRatio();
		void SetSizeMaxLimits(const glm::ivec2& max = { GLFW_DONT_CARE, GLFW_DONT_CARE });
		void ResetSizeMaxLimits();
		void SetSizeMinLimits(const glm::ivec2& min = { GLFW_DONT_CARE, GLFW_DONT_CARE });
		void ResetSizeMinLimits();
		void EnableResizability(bool enabled = true);
		void EnableDecorations(bool enabled = true);
		void EnableFloating(bool enabled = true);
		void Maximize();
#pragma endregion
#pragma endregion

#pragma region FULLSCREEN
#pragma region FULLSCREEN_GETTERS
		int GetRefreshRate() const;
#pragma endregion
#pragma region FULLSCREEN_SETTERS
		void SetRefreshRate(int refreshRate);
#pragma endregion
#pragma endregion

		void Use() const;
		void Update() const;
		void DrawEditor();
	};
}