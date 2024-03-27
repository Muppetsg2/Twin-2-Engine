#include <core/Window.h>
#include <core/Input.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine::Core;
using namespace std;
using namespace glm;

Window::Window(const string& title, const ivec2& size, bool fullscreen)
{
	GLFWmonitor* monitor = NULL;
	if (fullscreen) {
		monitor = glfwGetPrimaryMonitor();
		if (monitor == NULL) {
			spdlog::warn("Failed to create GLFW Monitor");
		}
	}

	_window = glfwCreateWindow(size.x, size.y, title.c_str(), monitor, NULL);
	if (_window == NULL)
	{
		spdlog::error("Failed to create GLFW Window!");
	}
	spdlog::info("Successfully created GLFW Window!");

	Input::InitForWindow(_window);
	Use();
}

Window::~Window()
{
	Input::FreeWindow(_window);
	glfwDestroyWindow(_window);
}

string Window::GetTitle() const
{
	return string(glfwGetWindowTitle(_window));
}

ivec2 Window::GetContentSize() const
{
	ivec2 size{};
	glfwGetWindowSize(_window, &size.x, &size.y);
	return size;
}

ivec2 Window::GetWindowSize() const
{
	ivec4 borders{};
	glfwGetWindowFrameSize(_window, &borders.x, &borders.y, &borders.z, &borders.w);
	ivec2 size = GetContentSize();
	size.x += borders.x + borders.z;
	size.y += borders.y + borders.w;
	return size;
}

bool Window::IsFullscreen() const
{
	return glfwGetWindowMonitor(_window) != NULL;
}

bool Window::IsWindowed() const
{
	return !isFullscreen();
}

GLFWmonitor* Window::GetMonitor() const
{
	return glfwGetWindowMonitor(_window);
}

GLFWwindow* Window::GetWindow() const
{
	return _window;
}

bool Window::IsClosed() const
{
	return glfwWindowShouldClose(_window);
}

ivec2 Window::GetWindowPos() const
{
	ivec2 pos{};
	glfwGetWindowPos(_window, &pos.x, &pos.y);
	return pos;
}

bool Window::IsMinimized() const
{
	return glfwGetWindowAttrib(_window, GLFW_ICONIFIED) == GLFW_TRUE;
}

bool Window::IsMaximized() const
{
	return glfwGetWindowAttrib(_window, GLFW_MAXIMIZED) == GLFW_TRUE;
}

bool Window::IsHidden() const
{
	return glfwGetWindowAttrib(_window, GLFW_VISIBLE) == GLFW_TRUE;
}

bool Window::IsFocused() const
{
	return glfwGetWindowAttrib(_window, GLFW_FOCUSED) == GLFW_TRUE;
}

bool Window::IsHovered() const
{
	return glfwGetWindowAttrib(_window, GLFW_HOVERED) == GLFW_TRUE;
}

bool Window::IsVisible() const
{
	return glfwGetWindowAttrib(_window, GLFW_VISIBLE) == GLFW_TRUE;
}

bool Window::IsResizable() const
{
	return glfwGetWindowAttrib(_window, GLFW_RESIZABLE) == GLFW_TRUE;
}

void Window::SetTitle(const string& title)
{
	glfwSetWindowTitle(_window, title.c_str());
}

void Window::SetWindowSize(const ivec2& size)
{
	glfwSetWindowSize(_window, size.x, size.y);
}

void Window::SetAspectRatio(const ivec2& ratio)
{
	glfwSetWindowAspectRatio(_window, ratio.x, ratio.y);
}

void Window::SetSizeMaxLimits(const ivec2& max)
{
	glfwSetWindowSizeLimits(_window, _minSizeLimits.x, _minSizeLimits.y, max.x, max.y);
	_maxSizeLimits = max;
}

void Window::SetSizeMinLimits(const ivec2& min)
{
	glfwSetWindowSizeLimits(_window, min.x, min.y, _maxSizeLimits.x, _maxSizeLimits.y);
	_minSizeLimits = min;
}

void Window::SetFullscreen(GLFWmonitor* monitor, const ivec2& size, int refreshRate)
{
	glfwSetWindowMonitor(_window, monitor, 0, 0, size.x, size.y, refreshRate);
}

void Window::SetWindowed(const ivec2& pos, const ivec2& size)
{
	glfwSetWindowMonitor(_window, NULL, pos.x, pos.y, size.x, size.y, 0);
}

// Enable VSync - fixes FPS at the refresh rate of your screen
void Window::EnableVSync(bool enabled)
{
	glfwSwapInterval(enabled ? 1 : 0);
}

void Window::SetWindowPos(const ivec2& pos)
{
	glfwSetWindowPos(_window, pos.x, pos.y);
}

void Window::SetWindowIcons(const std::vector<GLFWimage>& icons)
{
	glfwSetWindowIcon(_window, icons.size(), icons.data());
}

void Window::ResetWindowIcon()
{
	glfwSetWindowIcon(_window, 0, NULL);
}

void Window::Minimize()
{
	glfwIconifyWindow(_window);
}

void Window::Restore()
{
	glfwRestoreWindow(_window);
}

void Window::Maximize()
{
	glfwMaximizeWindow(_window);
}

void Window::Hide()
{
	glfwHideWindow(_window);
}

void Window::Show()
{
	glfwShowWindow(_window);
}

void Window::Focus()
{
	glfwFocusWindow(_window);
}

void Window::RequestAttention()
{
	glfwRequestWindowAttention(_window);
}

void Window::EnableResizability(bool enabled)
{
	glfwSetWindowAttrib(_window, GLFW_RESIZABLE, enabled ? GLFW_TRUE : GLFW_FALSE);
}

void Window::Use() const
{
	glfwMakeContextCurrent(_window);
}

void Window::Update() const
{
	Use();
	glfwSwapBuffers(_window);
}