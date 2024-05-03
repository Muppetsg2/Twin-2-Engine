#include <core/Input.h>
#include <graphic/Window.h>

using namespace std;
using namespace glm;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

GLFWwindow* Input::_mainWindow = nullptr;

vector<GLFWwindow*> Input::_windows;

map<GLFWwindow*, map<uint8_t, uint8_t>> Input::_mouseButtonStates;

map<GLFWwindow*, map<uint16_t, uint8_t>> Input::_keyStates;

void Input::key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) return; // Don't accept unknown keys
	uint16_t keyStateCode = key - KEY::SPACE;
	if (action == GLFW_PRESS) {
		_keyStates[win][keyStateCode] = INPUT_STATE::PRESSED;
	}
	else if (action == GLFW_RELEASE) {
		_keyStates[win][keyStateCode] = INPUT_STATE::RELEASED;
	}
	else if (action == GLFW_REPEAT) {
		_keyStates[win][keyStateCode] = INPUT_STATE::DOWN;
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

bool Input::IsInizializedForWindow(GLFWwindow* window) {
	for (size_t i = 0; i < _windows.size(); ++i) {
		if (_windows[i] == window) {
			return true;
		}
	}
	return false;
}

void Input::InitForWindow(GLFWwindow* window, bool mainWindow)
{
	// Init keys tables
	_keyStates[window] = map<uint16_t, uint8_t>();
	glfwSetKeyCallback(window, key_callback);
	// Init mouse tables
	_mouseButtonStates[window] = map<uint8_t, uint8_t>();
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	_windows.push_back(window);

	// Main Window
	if (mainWindow || _mainWindow == nullptr) SetMainWindow(window);
}

void Input::InitForWindow(Window* window, bool mainWindow)
{
	InitForWindow(window->GetWindow(), mainWindow);
}

void Input::FreeWindow(GLFWwindow* window)
{
	bool hasWindow = false;
	for (size_t i = 0; i < _windows.size(); ++i) {
		if (_windows[i] == window) {
			_windows.erase(_windows.begin() + i);
			hasWindow = true;
			break;
		}
	}

	if (hasWindow) {
		// Delete Keys Tables
		_keyStates.erase(window);

		// Delete Mouse Buttons Tables
		_mouseButtonStates.erase(window);
	}
}

void Input::FreeWindow(Window* window)
{
	FreeWindow(window->GetWindow());
}

void Input::FreeAllWindows()
{
	while (_windows.size() > 0) {
		FreeWindow(_windows[0]);
	}
}

void Input::SetMainWindow(GLFWwindow* window)
{
	if (IsInizializedForWindow(window)) 
		_mainWindow = window;
}

void Input::SetMainWindow(Window* window)
{
	SetMainWindow(window->GetWindow());
}

GLFWwindow* Input::GetMainWindow()
{
	return _mainWindow;
}

void Input::Update()
{
	for (const auto& windowKeyStates : _keyStates) {
		for (const auto& key : windowKeyStates.second) {
			if (key.second == INPUT_STATE::RELEASED) {
				_keyStates[windowKeyStates.first][key.first] = INPUT_STATE::UP;
			}
			else if (key.second == INPUT_STATE::PRESSED) {
				_keyStates[windowKeyStates.first][key.first] = INPUT_STATE::PRESSED_LONGER;
			}
		}
	}
	for (const auto& windowMouseButtonStates : _mouseButtonStates) {
		for (const auto& button : windowMouseButtonStates.second) {
			if (button.second == INPUT_STATE::RELEASED) {
				_mouseButtonStates[windowMouseButtonStates.first][button.first] = INPUT_STATE::UP;
			}
			else if (button.second == INPUT_STATE::PRESSED) {
				_mouseButtonStates[windowMouseButtonStates.first][button.first] = INPUT_STATE::PRESSED_LONGER;
			}
		}
	}
	glfwPollEvents();
}

void Input::HideAndLockCursor(GLFWwindow* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::HideCursor(GLFWwindow* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::KeepCursorInWindow(GLFWwindow* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

void Input::ShowCursor(GLFWwindow* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

CURSOR_STATE Input::GetCursorState(GLFWwindow* window)
{
	if (IsInizializedForWindow(window))
		return (CURSOR_STATE)glfwGetInputMode(window, GLFW_CURSOR);
	return CURSOR_STATE::DISABLED;
}

vec2 Input::GetMousePos(GLFWwindow* window)
{
	if (!IsInizializedForWindow(window))
		return vec2();
	double x = 0;
	double y = 0;
	glfwGetCursorPos(window, &x, &y);
	return vec2(x, y);
}

bool Input::IsMouseButtonPressed(GLFWwindow* window, MOUSE_BUTTON button)
{
	if (_mouseButtonStates.find(window) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[window].find(button) == _mouseButtonStates[window].end())
		return false;
	return _mouseButtonStates[window][button] == INPUT_STATE::PRESSED;
}

bool Input::IsMouseButtonReleased(GLFWwindow* window, MOUSE_BUTTON button)
{
	if (_mouseButtonStates.find(window) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[window].find(button) == _mouseButtonStates[window].end())
		return false;
	return _mouseButtonStates[window][button] == INPUT_STATE::RELEASED;
}

bool Input::IsMouseButtonDown(GLFWwindow* window, MOUSE_BUTTON button)
{
	if (_mouseButtonStates.find(window) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[window].find(button) == _mouseButtonStates[window].end())
		return false;
	INPUT_STATE currState = (INPUT_STATE)_mouseButtonStates[window][button];
	return currState == INPUT_STATE::PRESSED || currState == INPUT_STATE::PRESSED_LONGER || currState == INPUT_STATE::DOWN;
}

bool Input::IsMouseButtonHeldDown(GLFWwindow* window, MOUSE_BUTTON button)
{
	if (_mouseButtonStates.find(window) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[window].find(button) == _mouseButtonStates[window].end())
		return false;
	return _mouseButtonStates[window][button] == INPUT_STATE::DOWN;
}

bool Input::IsMouseButtonUp(GLFWwindow* window, MOUSE_BUTTON button)
{
	if (_mouseButtonStates.find(window) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[window].find(button) == _mouseButtonStates[window].end())
		return true;
	INPUT_STATE currState = (INPUT_STATE)_mouseButtonStates[window][button];
	return currState == INPUT_STATE::UP || currState == INPUT_STATE::RELEASED;
}

bool Input::IsMouseButtonHeldUp(GLFWwindow* window, MOUSE_BUTTON button)
{
	if (_mouseButtonStates.find(window) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[window].find(button) == _mouseButtonStates[window].end())
		return true;
	return _mouseButtonStates[window][button] == INPUT_STATE::UP;
}

bool Input::IsKeyPressed(GLFWwindow* window, KEY key)
{
	if (_keyStates.find(window) == _keyStates.end())
		return false;
	uint16_t keyStateCode = key - KEY::SPACE;
	if (_keyStates[window].find(keyStateCode) == _keyStates[window].end())
		return false;
	return _keyStates[window][keyStateCode] == INPUT_STATE::PRESSED;
}

bool Input::IsKeyReleased(GLFWwindow* window, KEY key)
{
	if (_keyStates.find(window) == _keyStates.end())
		return false;
	uint16_t keyStateCode = key - KEY::SPACE;
	if (_keyStates[window].find(keyStateCode) == _keyStates[window].end())
		return false;
	return _keyStates[window][keyStateCode] == INPUT_STATE::RELEASED;
}

bool Input::IsKeyDown(GLFWwindow* window, KEY key)
{
	if (_keyStates.find(window) == _keyStates.end())
		return false;
	uint16_t keyStateCode = key - KEY::SPACE;
	if (_keyStates[window].find(keyStateCode) == _keyStates[window].end())
		return false;
	INPUT_STATE currState = (INPUT_STATE)_keyStates[window][keyStateCode];
	return currState == INPUT_STATE::PRESSED || currState == INPUT_STATE::PRESSED_LONGER || currState == INPUT_STATE::DOWN;
}

bool Input::IsKeyHeldDown(GLFWwindow* window, KEY key)
{
	if (_keyStates.find(window) == _keyStates.end())
		return false;
	uint16_t keyStateCode = key - KEY::SPACE;
	if (_keyStates[window].find(keyStateCode) == _keyStates[window].end())
		return false;
	return _keyStates[window][keyStateCode] == INPUT_STATE::DOWN;
}

bool Input::IsKeyUp(GLFWwindow* window, KEY key)
{
	if (_keyStates.find(window) == _keyStates.end())
		return false;
	uint16_t keyStateCode = key - KEY::SPACE;
	if (_keyStates[window].find(keyStateCode) == _keyStates[window].end())
		return true;
	INPUT_STATE currState = (INPUT_STATE)_keyStates[window][keyStateCode];
	return currState == INPUT_STATE::RELEASED || currState == INPUT_STATE::UP;
}

bool Input::IsKeyHeldUp(GLFWwindow* window, KEY key)
{
	if (_keyStates.find(window) == _keyStates.end())
		return false;
	uint16_t keyStateCode = key - KEY::SPACE;
	if (_keyStates[window].find(keyStateCode) == _keyStates[window].end())
		return true;
	return _keyStates[window][keyStateCode] == INPUT_STATE::UP;
}

void Input::HideAndLockCursor(Window* window)
{
	return HideAndLockCursor(window->GetWindow());
}

void Input::HideCursor(Window* window)
{
	HideCursor(window->GetWindow());
}

void Input::KeepCursorInWindow(Window* window)
{
	KeepCursorInWindow(window->GetWindow());
}

void Input::ShowCursor(Window* window)
{
	ShowCursor(window->GetWindow());
}

CURSOR_STATE Input::GetCursorState(Window* window)
{
	return GetCursorState(window->GetWindow());
}

vec2 Input::GetMousePos(Window* window)
{
	return GetMousePos(window->GetWindow());
}

bool Input::IsMouseButtonPressed(Window* window, MOUSE_BUTTON button)
{
	return IsMouseButtonPressed(window->GetWindow(), button);
}

bool Input::IsMouseButtonReleased(Window* window, MOUSE_BUTTON button)
{
	return IsMouseButtonReleased(window->GetWindow(), button);
}

bool Input::IsMouseButtonDown(Window* window, MOUSE_BUTTON button)
{
	return IsMouseButtonDown(window->GetWindow(), button);
}

bool Input::IsMouseButtonHeldDown(Window* window, MOUSE_BUTTON button)
{
	return IsMouseButtonHeldDown(window->GetWindow(), button);
}

bool Input::IsMouseButtonUp(Window* window, MOUSE_BUTTON button)
{
	return IsMouseButtonUp(window->GetWindow(), button);
}

bool Input::IsMouseButtonHeldUp(Window* window, MOUSE_BUTTON button)
{
	return IsMouseButtonHeldUp(window->GetWindow(), button);
}

bool Input::IsKeyPressed(Window* window, KEY key)
{
	return IsKeyPressed(window->GetWindow(), key);
}

bool Input::IsKeyReleased(Window* window, KEY key)
{
	return IsKeyReleased(window->GetWindow(), key);
}

bool Input::IsKeyDown(Window* window, KEY key)
{
	return IsKeyDown(window->GetWindow(), key);
}

bool Input::IsKeyHeldDown(Window* window, KEY key)
{
	return IsKeyHeldDown(window->GetWindow(), key);
}

bool Input::IsKeyUp(Window* window, KEY key)
{
	return IsKeyUp(window->GetWindow(), key);
}

bool Input::IsKeyHeldUp(Window* window, KEY key)
{
	return IsKeyHeldUp(window->GetWindow(), key);
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

CURSOR_STATE Input::GetCursorState()
{
	return GetCursorState(_mainWindow);
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