#include <graphic/Window.h>

using namespace Twin2Engine::GraphicEngine;
using namespace std;
using namespace glm;

Window* Window::_instance = nullptr;
Twin2Engine::Core::MethodEventHandler Window::OnWindowSizeEvent;

Window::Window(const string& title, const ivec2& size, bool fullscreen)
{
	_window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
	if (_window == NULL)
	{
		spdlog::error("Failed to create GLFW Window!");
	}
	spdlog::info("Successfully created GLFW Window!");

	Use();

	if (fullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		if (monitor == NULL) {
			spdlog::warn("Failed to create GLFW Monitor");
			return;
		}
		SetFullscreen(monitor);
	}

	glfwSetFramebufferSizeCallback(this->GetWindow(), FramebufferSizeCallback);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Window::OnWindowSizeEvent();
}

Window::~Window()
{
	glfwDestroyWindow(_window);
}

Window* Window::MakeWindow(const string& title, const ivec2& size, bool fullscreen) {
	if (_instance != nullptr) {
		_instance->SetTitle(title);
		_instance->SetWindowSize(size);
		if (fullscreen) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			_instance->SetFullscreen(monitor);
		}
	}
	else {
		_instance = new Window(title, size, fullscreen);
	}

	return _instance;
}

Window* Window::GetInstance() {
	return _instance;
}

void Window::FreeAll() {
	delete _instance;
	_instance = nullptr;
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
	return !IsFullscreen();
}

GLFWmonitor* Window::GetMonitor() const
{
	return glfwGetWindowMonitor(_window);
}

GLFWwindow* Window::GetWindow() const
{
	return _window;
}

ivec2 Window::GetAspectRatio() const
{
	return _aspectRatio;
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

ivec2 Window::GetMaxSizeLimits() const
{
	return _maxSizeLimits;
}

ivec2 Window::GetMinSizeLimits() const
{
	return _minSizeLimits;
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

bool Window::IsDecorated() const
{
	return glfwGetWindowAttrib(_window, GLFW_DECORATED) == GLFW_TRUE;
}

bool Window::IsTransparent() const
{
	return glfwGetWindowAttrib(_window, GLFW_TRANSPARENT_FRAMEBUFFER) == GLFW_TRUE;
}

bool Window::IsVSyncOn() const
{
	return _vsyncOn;
}

float Window::GetOpacity() const
{
	return glfwGetWindowOpacity(_window);
}

bool Window::IsFloating() const
{
	return glfwGetWindowAttrib(_window, GLFW_FLOATING) == GLFW_TRUE;
}

bool Window::IsMousePassThrough() const
{
	return glfwGetWindowAttrib(_window, GLFW_MOUSE_PASSTHROUGH) == GLFW_TRUE;
}

int Window::GetRefreshRate() const
{
	return _refreshRate;
}

void Window::SetTitle(const string& title)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::SetTitle can't be used when window is in Fullscreen Mode");
		return;
	}
	glfwSetWindowTitle(_window, title.c_str());
}

void Window::SetWindowSize(const ivec2& size)
{
	glfwSetWindowSize(_window, size.x, size.y);
}

void Window::SetAspectRatio(const ivec2& ratio)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::SetAspectRatio can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowAspectRatio(_window, ratio.x, ratio.y);
	_aspectRatio = ratio;
}

void Window::ResetAspectRatio()
{
	if (IsFullscreen()) {
		spdlog::warn("Window::ResetAspectRatio can't be used when window is in Fullscreen mode");
		return;
	}
	SetAspectRatio({ GLFW_DONT_CARE, GLFW_DONT_CARE });
}

void Window::SetSizeMaxLimits(const ivec2& max)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::SetSizeMaxLimits can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowSizeLimits(_window, _minSizeLimits.x, _minSizeLimits.y, max.x, max.y);
	_maxSizeLimits = max;
}

void Window::ResetSizeMaxLimits()
{
	if (IsFullscreen()) {
		spdlog::warn("Window::ResetSizeMaxLimits can't be used when window is in Fullscreen mode");
		return;
	}
	SetSizeMaxLimits({ GLFW_DONT_CARE, GLFW_DONT_CARE });
}

void Window::SetSizeMinLimits(const ivec2& min)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::SetSizeMinLimits can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowSizeLimits(_window, min.x, min.y, _maxSizeLimits.x, _maxSizeLimits.y);
	_minSizeLimits = min;
}

void Window::ResetSizeMinLimits()
{
	if (IsFullscreen()) {
		spdlog::warn("Window::ResetSizeMinLimits can't be used when window is in Fullscreen mode");
		return;
	}
	SetSizeMinLimits({ GLFW_DONT_CARE, GLFW_DONT_CARE });
}

void Window::SetFullscreen(GLFWmonitor* monitor, const ivec2& size, int refreshRate)
{
	_refreshRate = refreshRate;
	glfwSetWindowMonitor(_window, monitor, 0, 0, size.x, size.y, refreshRate);
}

void Window::SetFullscreen(GLFWmonitor* monitor)
{
	const GLFWvidmode* vid = glfwGetVideoMode(monitor);
	SetFullscreen(monitor, { vid->width, vid->height }, vid->refreshRate);
}

void Window::SetWindowed(const ivec2& pos, const ivec2& size)
{
	glfwSetWindowMonitor(_window, NULL, pos.x, pos.y, size.x, size.y, 0);
}

void Window::SetWindowed()
{
	SetWindowed({ 0, 0 }, GetWindowSize());
}

void Window::EnableVSync(bool enabled)
{
	glfwSwapInterval(enabled ? 1 : 0);
	_vsyncOn = enabled;
}

void Window::SetWindowPos(const ivec2& pos)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::SetWindowPos can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowPos(_window, pos.x, pos.y);
}

void Window::SetWindowIcons(const std::vector<GLFWimage>& icons)
{
	glfwSetWindowIcon(_window, icons.size(), icons.data());
}

void Window::ResetWindowIcons()
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
	if (IsFullscreen()) {
		spdlog::warn("Window::Maximize can't be used when window is in Fullscreen mode");
		return;
	}
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

void Window::Close()
{
	glfwSetWindowShouldClose(_window, true);
}

void Window::RequestAttention()
{
	glfwRequestWindowAttention(_window);
}

void Window::EnableResizability(bool enabled)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::EnableResizability can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowAttrib(_window, GLFW_RESIZABLE, enabled ? GLFW_TRUE : GLFW_FALSE);
}

void Window::EnableDecorations(bool enabled)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::EnableDecorations can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowAttrib(_window, GLFW_DECORATED, enabled ? GLFW_TRUE : GLFW_FALSE);
}

void Window::EnableTransparency(bool enabled)
{
	glfwSetWindowAttrib(_window, GLFW_TRANSPARENT_FRAMEBUFFER, enabled ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SetOpacity(float opacity)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::SetOpacity can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowOpacity(_window, opacity);
}

void Window::EnableFloating(bool enabled)
{
	if (IsFullscreen()) {
		spdlog::warn("Window::EnableFloating can't be used when window is in Fullscreen mode");
		return;
	}
	glfwSetWindowAttrib(_window, GLFW_FLOATING, enabled ? GLFW_TRUE : GLFW_FALSE);
}

void Window::EnableMousePassThrough(bool enabled)
{
	glfwSetWindowAttrib(_window, GLFW_MOUSE_PASSTHROUGH, enabled ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SetRefreshRate(int refreshRate)
{
	if (IsWindowed()) {
		spdlog::warn("Window::SetRefreshRate can't be used when window is in Windowed mode");
		return;
	}
	SetFullscreen(GetMonitor(), GetWindowSize(), refreshRate);
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