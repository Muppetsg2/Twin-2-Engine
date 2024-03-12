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
	std::string atype = "";
	if (action == GLFW_PRESS) {
		_keyStates[win][key] = INPUT_STATE::PRESSED;
		atype = "Press";
	}
	else if (action == GLFW_RELEASE) {
		_keyStates[win][key] = INPUT_STATE::RELEASED;
		atype = "Released";
	}
	else if (action == GLFW_REPEAT) {
		atype = "Repeat";
	}

	printf(std::string("Action: "s + atype + "\n"s).c_str());
	/*else if (action == GLFW_REPEAT) {
		if (isKeyPressed(win, key)) {
			_keyStates[win][key] |= INPUT_STATE::HOLD;
		}
	}*/
}

void Input::mouse_button_callback(GLFWwindow* win, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (isMouseButtonPressed(win, button)) {
			_mouseButtonStates[win][button] |= INPUT_STATE::HOLD;
		}
		else {
			_mouseButtonStates[win][button] = INPUT_STATE::PRESSED;
		}
	}
	else if (action == GLFW_RELEASE) {
		_mouseButtonStates[win][button] = INPUT_STATE::RELEASED;
	}
}

void Input::initForWindow(GLFWwindow* window, bool mainWindow)
{
	// Init keys tables
	_keyStates[window] = new int[KEYS_MAX_NUM](INPUT_STATE::NONE);
	glfwSetKeyCallback(window, key_callback);
	// Init mouse tables
	_mouseButtonStates[window] = new int[MOUSE_BUTTONS_MAX_NUM](INPUT_STATE::NONE);
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
	return (_mouseButtonStates[window][button] & INPUT_STATE::PRESSED) != 0;
}

bool Input::isMouseButtonReleased(GLFWwindow* window, int button)
{
	return (_mouseButtonStates[window][button] & INPUT_STATE::RELEASED) != 0;
}

bool Input::isMouseButtonHold(GLFWwindow* window, int button)
{
	return (_mouseButtonStates[window][button] & INPUT_STATE::HOLD) != 0;
}

bool Input::isKeyPressed(GLFWwindow* window, int key)
{
	//return (_keyStates[window][key] & INPUT_STATE::PRESSED) != 0;
	int state = glfwGetKey(window, key);
	return state == GLFW_PRESS;
}

bool Input::isKeyReleased(GLFWwindow* window, int key)
{
	//return (_keyStates[window][key] & INPUT_STATE::RELEASED) != 0;
	int state = glfwGetKey(window, key);
	return state == GLFW_RELEASE;
}

bool Input::isKeyHold(GLFWwindow* window, int key)
{
	//return (_keyStates[window][key] & INPUT_STATE::HOLD) != 0;
	int state = glfwGetKey(window, key);
	return (_keyStates[window][key] & INPUT_STATE::PRESSED) != 0 && state == GLFW_PRESS;
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