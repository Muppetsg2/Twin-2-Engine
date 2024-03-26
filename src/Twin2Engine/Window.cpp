#include <core/Window.h>

using namespace Twin2Engine::Core;
using namespace std;

Window::Window(const string& title, unsigned int width, unsigned int height, bool fullscreen)
{
}

Window::~Window()
{
}

string Window::GetTitle() const
{
	return std::string();
}

unsigned int Window::GetHeight() const
{
	return 0;
}

unsigned int Window::GetWidth() const
{
	return 0;
}

bool Window::isFullscreen() const
{
	return false;
}

GLFWmonitor* Window::GetMonitor() const
{
	return nullptr;
}

GLFWwindow* Window::GetWindow() const
{
	return nullptr;
}

void Window::SetTitle(const std::string& title)
{
}

void Window::SetHeight(unsigned int height)
{
}

void Window::SetWidth(unsigned int width)
{
}

void Window::SetFullscreen(bool fullscreen)
{
}

void Window::SetMonitor()
{
}

void Window::Use() const
{
}

void Window::Update() const
{
	Use();
}