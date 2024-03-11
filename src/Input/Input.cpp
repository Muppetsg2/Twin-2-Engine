#include "Input/Input.h"

using namespace std;

GLFWwindow* Input::_mainWindow = nullptr;

map<GLFWwindow*, bool*> Input::_mouseButtonPressed = map<GLFWwindow*, bool*>();
map<GLFWwindow*, bool*> Input::_mouseButtonHolded = map<GLFWwindow*, bool*>();

map<GLFWwindow*, bool*> Input::_keyPressed = map<GLFWwindow*, bool*>();
map<GLFWwindow*, bool*> Input::_keyHolded = map<GLFWwindow*, bool*>();

void Input::key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) return; // Don't accept unknown keys
	if (action == GLFW_PRESS) {
		if (_keyPressed[win][key]) {
			_keyHolded[win][key] = true;
		}
		else {
			_keyPressed[win][key] = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		_keyPressed[win][key] = false;
		_keyHolded[win][key] = false;
	}
}

void Input::mouse_button_callback(GLFWwindow* win, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (_mouseButtonPressed[win][button]) {
			_mouseButtonHolded[win][button] = true;
		}
		else {
			_mouseButtonPressed[win][button] = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		_mouseButtonPressed[win][button] = false;
		_mouseButtonHolded[win][button] = false;
	}
}

void Input::initForWindow(GLFWwindow* window, bool mainWindow)
{
	// Init keys tables
	_keyPressed[window] = new bool[KEYS_MAX_NUM];
	_keyHolded[window] = new bool[KEYS_MAX_NUM];
	glfwSetKeyCallback(window, key_callback);
	// Init mouse tables
	_mouseButtonPressed[window] = new bool[KEYS_MAX_NUM];
	_mouseButtonHolded[window] = new bool[KEYS_MAX_NUM];
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Main Window
	if (mainWindow || _mainWindow == nullptr) setMainWindow(window);
}

void Input::freeWindow(GLFWwindow* window)
{
	// Delete Keys Tables
	delete[] _keyPressed[window];
	delete[] _keyHolded[window];
	_keyPressed.erase(window);
	_keyHolded.erase(window);

	// Delete Mouse Buttons Tables
	delete[] _mouseButtonPressed[window];
	delete[] _mouseButtonHolded[window];
	_mouseButtonPressed.erase(window);
	_mouseButtonHolded.erase(window);
}

constexpr void Input::setMainWindow(GLFWwindow* window)
{
	_mainWindow = window;
}

constexpr GLFWwindow* Input::getMainWindow()
{
	return _mainWindow;
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

bool Input::isMouseButtonPressed(GLFWwindow* window, int button)
{
	return _mouseButtonPressed[window][button];
}

bool Input::isMouseButtonReleased(GLFWwindow* window, int button)
{
	return !_mouseButtonPressed[window][button];
}

bool Input::isMouseButtonHold(GLFWwindow* window, int button)
{
	return _mouseButtonHolded[window][button];
}

bool Input::isKeyPressed(GLFWwindow* window, int key)
{
	return _keyPressed[window][key];
}

bool Input::isKeyReleased(GLFWwindow* window, int key)
{
	return !_keyPressed[window][key];
}

bool Input::isKeyHold(GLFWwindow* window, int key)
{
	return _keyHolded[window][key];
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

bool Input::isMouseButtonPressed(int button)
{
	return isMouseButtonPressed(_mainWindow, button);
}

bool Input::isMouseButtonReleased(int button)
{
	return isMouseButtonReleased(_mainWindow, button);
}

bool Input::isMouseButtonHold(int button)
{
	return isMouseButtonHold(_mainWindow, button);
}

bool Input::isKeyPressed(int key)
{
	return isKeyPressed(_mainWindow, key);
}

bool Input::isKeyReleased(int key)
{
	return isKeyReleased(_mainWindow, key);
}

bool Input::isKeyHold(int key)
{
	return isKeyHold(_mainWindow, key);
}