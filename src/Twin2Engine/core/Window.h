#pragma once
#include <GLFW/glfw3.h>
#include <string>

namespace Twin2Engine {
	namespace Core {
		class Window {
		private:
			GLFWmonitor* _monitor = NULL;
			GLFWwindow* _window;
		public:
			Window(const std::string& title, unsigned int width, unsigned int height, bool fullscreen = false);
			virtual ~Window();

			std::string GetTitle() const;
			unsigned int GetHeight() const;
			unsigned int GetWidth() const;
			bool isFullscreen() const;
			GLFWmonitor* GetMonitor() const;
			GLFWwindow* GetWindow() const;

			void SetTitle(const std::string& title);
			void SetHeight(unsigned int height);
			void SetWidth(unsigned int width);
			void SetFullscreen(bool fullscreen);
			void SetMonitor();

			void Use() const;
			void Update() const;
		};
	}
}