#include "Input/Input.h"
#include <string>

using namespace std;

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

void Input::initForWindow(GLFWwindow* window, bool mainWindow)
{
	// Init keys tables
	_keyStates[window] = new int[KEY::KEYS_SIZE]();
	glfwSetKeyCallback(window, key_callback);
	// Init mouse tables
	_mouseButtonStates[window] = new int[MOUSE_BUTTON::MOUSE_BUTTONS_SIZE]();
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	_windows.push_back(window);

	// Main Window
	if (mainWindow || _mainWindow == nullptr) setMainWindow(window);
}

void Input::freeWindow(GLFWwindow* window)
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

void Input::freeAllWindows()
{
	while (_windows.size() > 0) {
		freeWindow(_windows[0]);
	}
}

constexpr void Input::setMainWindow(GLFWwindow* window)
{
	_mainWindow = window;
}

constexpr GLFWwindow* Input::getMainWindow()
{
	return _mainWindow;
}

void Input::pollEvents()
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

void Input::hideAndLockCursor(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::hideCursor(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::keepCursorInWindow(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

void Input::showCursor(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

glm::vec2 Input::getMousePos(GLFWwindow* window)
{
	double x = 0;
	double y = 0;
	glfwGetCursorPos(window, &x, &y);
	return glm::vec2(x, y);
}

bool Input::isMouseButtonPressed(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::PRESSED;
}

bool Input::isMouseButtonReleased(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::RELEASED;
}

bool Input::isMouseButtonDown(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::PRESSED || _mouseButtonStates[window][button] == INPUT_STATE::DOWN;
}

bool Input::isMouseButtonHeldDown(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::DOWN;
}

bool Input::isMouseButtonUp(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::UP || _mouseButtonStates[window][button] == INPUT_STATE::RELEASED;
}

bool Input::isMouseButtonHeldUp(GLFWwindow* window, MOUSE_BUTTON button)
{
	return _mouseButtonStates[window][button] == INPUT_STATE::UP;
}

bool Input::isKeyPressed(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::PRESSED;
}

bool Input::isKeyReleased(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::RELEASED;
}

bool Input::isKeyDown(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::PRESSED || _keyStates[window][key] == INPUT_STATE::DOWN;
}

bool Input::isKeyHeldDown(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::DOWN;
}

bool Input::isKeyUp(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::RELEASED || _keyStates[window][key] == INPUT_STATE::UP;
}

bool Input::isKeyHeldUp(GLFWwindow* window, KEY key)
{
	return _keyStates[window][key] == INPUT_STATE::UP;
}

void Input::hideAndLockCursor()
{
	hideAndLockCursor(_mainWindow);
}

void Input::hideCursor()
{
	hideCursor(_mainWindow);
}

void Input::keepCursorInWindow()
{
	keepCursorInWindow(_mainWindow);
}

void Input::showCursor()
{
	showCursor(_mainWindow);
}

glm::vec2 Input::getMousePos()
{
	return getMousePos(_mainWindow);
}

bool Input::isMouseButtonPressed(MOUSE_BUTTON button)
{
	return isMouseButtonPressed(_mainWindow, button);
}

bool Input::isMouseButtonReleased(MOUSE_BUTTON button)
{
	return isMouseButtonReleased(_mainWindow, button);
}

bool Input::isMouseButtonDown(MOUSE_BUTTON button)
{
	return isMouseButtonDown(_mainWindow, button);
}

bool Input::isMouseButtonHeldDown(MOUSE_BUTTON button)
{
	return isMouseButtonHeldDown(_mainWindow, button);
}

bool Input::isMouseButtonUp(MOUSE_BUTTON button)
{
	return isMouseButtonUp(_mainWindow, button);
}

bool Input::isMouseButtonHeldUp(MOUSE_BUTTON button)
{
	return isMouseButtonHeldUp(_mainWindow, button);
}

bool Input::isKeyPressed(KEY key)
{
	return isKeyPressed(_mainWindow, key);
}

bool Input::isKeyReleased(KEY key)
{
	return isKeyReleased(_mainWindow, key);
}

bool Input::isKeyDown(KEY key)
{
	return isKeyDown(_mainWindow, key);
}

bool Input::isKeyHeldDown(KEY key)
{
	return isKeyHeldDown(_mainWindow, key);
}

bool Input::isKeyUp(KEY key)
{
	return isKeyUp(_mainWindow, key);
}

bool Input::isKeyHeldUp(KEY key)
{
	return isKeyHeldUp(_mainWindow, key);
}