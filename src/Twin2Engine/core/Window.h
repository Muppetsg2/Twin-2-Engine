#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>

namespace Twin2Engine {
	namespace Core {
		class Window {
		private:
			GLFWwindow* _window;
			glm::ivec2 _minSizeLimits = { GLFW_DONT_CARE, GLFW_DONT_CARE };
			glm::ivec2 _maxSizeLimits = { GLFW_DONT_CARE, GLFW_DONT_CARE };
			int _refreshRate = 0;

		public:
			Window(const std::string& title, const glm::ivec2& size, bool fullscreen = false);
			virtual ~Window();

#pragma region GLOBAL
#pragma region GLOBAL_GETTERS
			std::string GetTitle() const;
			glm::ivec2 GetContentSize() const;
			glm::ivec2 GetWindowSize() const;
			GLFWmonitor* GetMonitor() const;
			GLFWwindow* GetWindow() const;
			bool IsFullscreen() const;
			bool IsWindowed() const;
			bool IsClosed() const;
			bool IsMinimized() const;
			bool IsHidden() const;
			bool IsFocused() const;
			bool IsHovered() const;
			bool IsVisible() const;
			bool IsTransparent() const;
#pragma endregion
#pragma region GLOBAL_SETTERS

#pragma endregion
#pragma endregion

#pragma region WINDOWED
#pragma region WINDOWED_GETTERS
			glm::ivec2 GetWindowPos() const;
			float GetOpacity() const;
			bool IsMaximized() const;
			bool IsResizable() const;
			bool IsDecorated() const;
			bool IsFloating() const;
			bool IsMousePassThrought() const;
#pragma endregion
#pragma region WINDOWED_SETTERS

#pragma endregion
#pragma endregion
			
#pragma region FULLSCREEN
#pragma region FULLSCREEN_GETTERS
			int GetRefreshRate() const;
#pragma endregion
#pragma endregion

			void SetTitle(const std::string& title);
			void SetWindowSize(const glm::ivec2& size); // Sprawdziæ dla Fullscreen
			void SetAspectRatio(const glm::ivec2& ratio); // Sprawdziæ dla Fullscreen
			void SetSizeMaxLimits(const glm::ivec2& max = { GLFW_DONT_CARE, GLFW_DONT_CARE }); // Sprawdziæ dla Fullscreen
			void SetSizeMinLimits(const glm::ivec2& min = { GLFW_DONT_CARE, GLFW_DONT_CARE }); // Sprawdziæ dla Fullscreen
			void SetFullscreen(GLFWmonitor* monitor, const glm::ivec2& size, int refreshRate = 60);
			void SetFullscreen(GLFWmonitor* monitor);
			void SetWindowed(const glm::ivec2& pos, const glm::ivec2& size);
			void SetWindowed();
			void EnableVSync(bool enabled = true); // Sprawdziæ
			void SetWindowPos(const glm::ivec2& pos);
			void SetWindowIcons(const std::vector<GLFWimage>& icons);
			void ResetWindowIcon();
			void Minimize();
			void Restore();
			void Maximize();
			void Hide();
			void Show();
			void Focus();
			void RequestAttention();
			void EnableResizability(bool enabled = true);
			void EnableDecorations(bool enabled = true);
			void EnableTransparency(bool enabled = true);
			void SetOpacity(float opacity);
			void EnableFloating(bool enabled = true);
			void EnableMousePassThrought(bool enabled = true); // Sprawdziæ dla Fullscreen
			void SetRefreshRate(int refreshRate);

			void Use() const;
			void Update() const;
		};
	}
}