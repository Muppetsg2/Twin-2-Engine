#include <core/Input.h>
#include <string>

using namespace std;
using namespace glm;
using namespace Twin2Engine::Core;

GLFWwindow* Input::_mainWindow = nullptr;

std::vector<GLFWwindow*> Input::_windows = std::vector<GLFWwindow*>();

map<GLFWwindow*, int*> Input::_mouseButtonStates = map<GLFWwindow*, int*>();

map<GLFWwindow*, int*> Input::_keyStates = map<GLFWwindow*, int*>();

void Input::key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) return; // Don't accept unknown keys
	if (action == GLFW_PRESS) {
		_keyStates[win][key] = INPUT_STATE::PRESSED;
	}
	else if (action == GLFW_RELEASE) {
		_keyStates[win][key] = INPUT_STATE::RELEASED;
	}
	else if (action == GLFW_REPEAT) {
		_keyStates[win][key] = INPUT_STATE::DOWN;
	}
}

void Input::mouse_button_callback(GLFWwindow* win, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		_mouseButtonStates[win][button] = INPUT_STATE::PRESSED;
	}
	else if (action == GLFW_RELEASE) {
		_mouseButtonStates[win][button] = INPUT_STATE::RELEASED;
	}
	else if (action == GLFW_REPEAT) {
		_mouseButtonStates[win][button] = INPUT_STATE::DOWN;
	}
}

void Input::InitForWindow(GLFWwindow* window, bool mainWindow)
{
	// Init keys tables
	_keyStates[window] = new int[KEY::KEYS_SIZE]();
	glfwSetKeyCallback(window, key_callback);
	// Init mouse tables
	_mouseButtonStates[window] = new int[MOUSE_BUTTON::MOUSE_BUTTONS_SIZE]();
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	_windows.push_back(window);

	// Main Window
	if (mainWindow || _mainWindow == nullptr) SetMainWindow(window);
}

void Input::FreeWindow(GLFWwindow* window)
{
	// Delete Keys Tables
	delete[] _keyStates[window];
	_keyStates.erase(window);

	// Delete Mouse Buttons Tables
	delete[] _mouseButtonStates[window];
	_mouseButtonStates.erase(window);

	for (size_t i = 0; i < _windows.size(); ++i) {
		if (_windows[i] == window) {
			_windows.erase(_windows.begin() + i);
			break;
		}
	}
}

void Input::FreeAllWindows()
{
	while (_windows.size() > 0) {
		FreeWindow(_windows[0]);
	}
}

constexpr void Input::SetMainWindow(GLFWwindow* window)
{
	_mainWindow = window;
}

constexpr GLFWwindow* Input::GetMainWindow()
{
	return _mainWindow;
}

void Input::Update()
{
	for (const auto& keyStates : _keyStates) {
		for (size_t i = 0; i < KEY::KEYS_SIZE; ++i) {
			if (keyStates.second[i] == INPUT_STATE::RELEASED) {
				keyStates.second[i] = INPUT_STATE::UP;
			}
			if (keyStates.second[i] == INPUT_STATE::PRESSED) {
				keyStates.second[i] = INPUT_STATE::PRESSED_LONGER;
			}
		}
	}
	for (const auto& mouseButtonStates : _mouseButtonStates) {
		for (size_t i = 0; i < MOUSE_BUTTON::MOUSE_BUTTONS_SIZE; ++i) {
			if (mouseButtonStates.second[i] == INPUT_STATE::RELEASED) {
				mouseButtonStates.second[i] = INPUT_STATE::UP;
			}
			if (mouseButtonStates.second[i] == INPUT_STATE::PRESSED) {
				mouseButtonStates.second[i] = INPUT_STATE::PRESSED_LONGER;
			}
		}
	}
	glfwPollEvents();
}

void Input::HideAndLockCursor(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::HideCursor(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::KeepCursorInWindow(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

void Input::ShowCursor(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

vec2 Input::GetMousePos(GLFWwindow* window)
{
	double x = 0;
	double y = 0;
	glfwGetCursorPos(window, &x, &y);
	return vec2(x, y);
}

bool Input::IsMouseButtonPressed(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::PRESSED;
}

bool Input::IsMouseButtonReleased(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::RELEASED;
}

bool Input::IsMouseButtonDown(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::PRESSED || _mouseButtonStates[window][button] == INPUT_STATE::DOWN;
}

bool Input::IsMouseButtonHeldDown(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::DOWN;
}

bool Input::IsMouseButtonUp(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::UP || _mouseButtonStates[window][button] == INPUT_STATE::RELEASED;
}

bool Input::IsMouseButtonHeldUp(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::UP;
}

bool Input::IsKeyPressed(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::PRESSED;
}

bool Input::IsKeyReleased(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::RELEASED;
}

bool Input::IsKeyDown(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::PRESSED || _keyStates[window][key] == INPUT_STATE::DOWN;
}

bool Input::IsKeyHeldDown(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::DOWN;
}

bool Input::IsKeyUp(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::RELEASED || _keyStates[window][key] == INPUT_STATE::UP;
}

bool Input::IsKeyHeldUp(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::UP;
}

void Input::HideAndLockCursor()
{
	HideAndLockCursor(_mainWindow);
}

void Input::HideCursor()
{
	HideCursor(_mainWindow);
}

void Input::KeepCursorInWindow()
{
	KeepCursorInWindow(_mainWindow);
}

void Input::ShowCursor()
{
	ShowCursor(_mainWindow);
}

vec2 Input::GetMousePos()
{
	return GetMousePos(_mainWindow);
}

bool Input::IsMouseButtonPressed(MOUSE_BUTTON button)
{
	return IsMouseButtonPressed(_mainWindow, button);
}

bool Input::IsMouseButtonReleased(MOUSE_BUTTON button)
{
	return IsMouseButtonReleased(_mainWindow, button);
}

bool Input::IsMouseButtonDown(MOUSE_BUTTON button)
{
	return IsMouseButtonDown(_mainWindow, button);
}

bool Input::IsMouseButtonHeldDown(MOUSE_BUTTON button)
{
	return IsMouseButtonHeldDown(_mainWindow, button);
}

bool Input::IsMouseButtonUp(MOUSE_BUTTON button)
{
	return IsMouseButtonUp(_mainWindow, button);
}

bool Input::IsMouseButtonHeldUp(MOUSE_BUTTON button)
{
	return IsMouseButtonHeldUp(_mainWindow, button);
}

bool Input::IsKeyPressed(KEY key)
{
	return IsKeyPressed(_mainWindow, key);
}

bool Input::IsKeyReleased(KEY key)
{
	return IsKeyReleased(_mainWindow, key);
}

bool Input::IsKeyDown(KEY key)
{
	return IsKeyDown(_mainWindow, key);
}

bool Input::IsKeyHeldDown(KEY key)
{
	return IsKeyHeldDown(_mainWindow, key);
}

bool Input::IsKeyUp(KEY key)
{
	return IsKeyUp(_mainWindow, key);
}

bool Input::IsKeyHeldUp(KEY key)
{
	return IsKeyHeldUp(_mainWindow, key);
}