#include <core/Input.h>
#include <graphic/Window.h>

using namespace std;
using namespace glm;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Tools;

Window* Input::_mainWindow = nullptr;

vector<Window*> Input::_windows;

map<GLFWwindow*, map<uint8_t, uint8_t>> Input::_mouseButtonStates;

map<GLFWwindow*, map<uint16_t, uint8_t>> Input::_keyStates;

map<GLFWwindow*, EventHandler<KEY, INPUT_STATE, KEY_MOD>> Input::_onKeyStateChange;
map<GLFWwindow*, EventHandler<unsigned int>> Input::_onTextInput;
map<GLFWwindow*, EventHandler<glm::vec2>> Input::_onCursorPosChange;
map<GLFWwindow*, EventHandler<MOUSE_BUTTON, INPUT_STATE>> Input::_onMouseButtonStateChange;

void Input::key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) return; // Don't accept unknown keys
	uint16_t keyStateCode = (uint16_t)key - (uint16_t)KEY::SPACE;
	if (action == GLFW_PRESS) {
		_keyStates[win][keyStateCode] = (uint8_t)INPUT_STATE::PRESSED;
		_onKeyStateChange[win].Invoke((KEY)key, INPUT_STATE::PRESSED, (KEY_MOD)mods);
	}
	else if (action == GLFW_RELEASE) {
		_keyStates[win][keyStateCode] = (uint8_t)INPUT_STATE::RELEASED;
		_onKeyStateChange[win].Invoke((KEY)key, INPUT_STATE::RELEASED, (KEY_MOD)mods);
	}
	else if (action == GLFW_REPEAT) {
		_keyStates[win][keyStateCode] = (uint8_t)INPUT_STATE::DOWN;
		_onKeyStateChange[win].Invoke((KEY)key, INPUT_STATE::DOWN, (KEY_MOD)mods);
	}
}

void Input::text_input_callback(GLFWwindow* win, unsigned int codepoint)
{
	_onTextInput[win].Invoke(codepoint);
}

void Input::mouse_button_callback(GLFWwindow* win, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		_mouseButtonStates[win][button] = (uint8_t)INPUT_STATE::PRESSED;
		_onMouseButtonStateChange[win].Invoke((MOUSE_BUTTON)button, INPUT_STATE::PRESSED);
	}
	else if (action == GLFW_RELEASE) {
		_mouseButtonStates[win][button] = (uint8_t)INPUT_STATE::RELEASED;
		_onMouseButtonStateChange[win].Invoke((MOUSE_BUTTON)button, INPUT_STATE::RELEASED);
	}
	else if (action == GLFW_REPEAT) {
		_mouseButtonStates[win][button] = (uint8_t)INPUT_STATE::DOWN;
		_onMouseButtonStateChange[win].Invoke((MOUSE_BUTTON)button, INPUT_STATE::DOWN);
	}
}

void Input::cursor_pos_callback(GLFWwindow* win, double xpos, double ypos)
{
	vec2 windowSize = GetWindow(win)->GetContentSize();
	_onCursorPosChange[win].Invoke({ xpos - windowSize.x * .5f, -ypos + windowSize.y * .5f });
}

bool Input::IsInizializedForWindow(Graphic::Window* window)
{
	for (size_t i = 0; i < _windows.size(); ++i) {
		if (_windows[i] == window) {
			return true;
		}
	}
	return false;
}

Window* Input::GetWindow(GLFWwindow* window)
{
	for (auto& win : _windows) {
		if (win->GetWindow() == window) {
			return win;
		}
	}
	return nullptr;
}

void Input::InitForWindow(Window* window, bool mainWindow)
{
	GLFWwindow* glfwWin = window->GetWindow();
	// Init keys tables
	_keyStates[glfwWin] = map<uint16_t, uint8_t>();
	glfwSetInputMode(glfwWin, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSetKeyCallback(glfwWin, key_callback);
	// Init mouse tables
	_mouseButtonStates[glfwWin] = map<uint8_t, uint8_t>();
	glfwSetMouseButtonCallback(glfwWin, mouse_button_callback);

	// Text Input Event
	glfwSetCharCallback(glfwWin, text_input_callback);

	// Cursor Pos Event
	glfwSetCursorPosCallback(glfwWin, cursor_pos_callback);

	_windows.push_back(window);

	// Main Window
	if (mainWindow || _mainWindow == nullptr) SetMainWindow(window);
}

void Input::FreeWindow(Window* window)
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
		_keyStates.erase(window->GetWindow());

		// Delete Mouse Buttons Tables
		_mouseButtonStates.erase(window->GetWindow());

		// Delete Event Handlers
		_onKeyStateChange.erase(window->GetWindow());
		_onMouseButtonStateChange.erase(window->GetWindow());
		_onCursorPosChange.erase(window->GetWindow());
		_onTextInput.erase(window->GetWindow());
	}
}

void Input::FreeAllWindows()
{
	while (_windows.size() > 0) {
		FreeWindow(_windows[0]);
	}
}

void Input::SetMainWindow(Window* window)
{
	if (IsInizializedForWindow(window))
		_mainWindow = window;
}

Window* Input::GetMainWindow()
{
	return _mainWindow;
}

void Input::Update()
{
	for (const auto& windowKeyStates : _keyStates) {
		for (const auto& key : windowKeyStates.second) {
			if (key.second == (uint8_t)INPUT_STATE::RELEASED) {
				_keyStates[windowKeyStates.first][key.first] = (uint8_t)INPUT_STATE::UP;
				_onKeyStateChange[windowKeyStates.first].Invoke((KEY)(key.first + (uint16_t)KEY::SPACE), INPUT_STATE::UP, KEY_MOD::NONE);
			}
			else if (key.second == (uint8_t)INPUT_STATE::PRESSED) {
				_keyStates[windowKeyStates.first][key.first] = (uint8_t)INPUT_STATE::PRESSED_LONGER;
			}
		}
	}
	for (const auto& windowMouseButtonStates : _mouseButtonStates) {
		for (const auto& button : windowMouseButtonStates.second) {
			if (button.second == (uint8_t)INPUT_STATE::RELEASED) {
				_mouseButtonStates[windowMouseButtonStates.first][button.first] = (uint8_t)INPUT_STATE::UP;
				_onMouseButtonStateChange[windowMouseButtonStates.first].Invoke((MOUSE_BUTTON)button.first, INPUT_STATE::UP);
			}
			else if (button.second == (uint8_t)INPUT_STATE::PRESSED) {
				_mouseButtonStates[windowMouseButtonStates.first][button.first] = (uint8_t)INPUT_STATE::PRESSED_LONGER;
			}
		}
	}
	glfwPollEvents();
}

string Input::GetKeyName(KEY key)
{
	return string(glfwGetKeyName((int)key, 0));
}

void Input::HideAndLockCursor(Window* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::HideCursor(Window* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::KeepCursorInWindow(Window* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

void Input::ShowCursor(Window* window)
{
	if (IsInizializedForWindow(window))
		glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

CURSOR_STATE Input::GetCursorState(Window* window)
{
	if (IsInizializedForWindow(window))
		return (CURSOR_STATE)glfwGetInputMode(window->GetWindow(), GLFW_CURSOR);
	return CURSOR_STATE::DISABLED;
}

vec2 Input::GetCursorPos(Window* window)
{
	if (!IsInizializedForWindow(window))
		return vec2();
	double x = 0;
	double y = 0;
	glfwGetCursorPos(window->GetWindow(), &x, &y);
	vec2 windowSize = window->GetContentSize();
	x -= windowSize.x * .5f;
	y = -y + windowSize.y * .5f;
	return vec2(x, y);
}

EventHandler<vec2>& Input::GetOnCursorPosChange(Window* window)
{
	return _onCursorPosChange[window->GetWindow()];
}

bool Input::IsMouseButtonPressed(Window* window, MOUSE_BUTTON button)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_mouseButtonStates.find(glfwWin) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[glfwWin].find((uint8_t)button) == _mouseButtonStates[glfwWin].end())
		return false;
	return _mouseButtonStates[glfwWin][(uint8_t)button] == (uint8_t)INPUT_STATE::PRESSED;
}

bool Input::IsMouseButtonReleased(Window* window, MOUSE_BUTTON button)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_mouseButtonStates.find(glfwWin) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[glfwWin].find((uint8_t)button) == _mouseButtonStates[glfwWin].end())
		return false;
	return _mouseButtonStates[glfwWin][(uint8_t)button] == (uint8_t)INPUT_STATE::RELEASED;
}

bool Input::IsMouseButtonDown(Window* window, MOUSE_BUTTON button)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_mouseButtonStates.find(glfwWin) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[glfwWin].find((uint8_t)button) == _mouseButtonStates[glfwWin].end())
		return false;
	INPUT_STATE currState = (INPUT_STATE)_mouseButtonStates[glfwWin][(uint8_t)button];
	return currState == INPUT_STATE::PRESSED || currState == INPUT_STATE::PRESSED_LONGER || currState == INPUT_STATE::DOWN;
}

bool Input::IsMouseButtonHeldDown(Window* window, MOUSE_BUTTON button)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_mouseButtonStates.find(glfwWin) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[glfwWin].find((uint8_t)button) == _mouseButtonStates[glfwWin].end())
		return false;
	return _mouseButtonStates[glfwWin][(uint8_t)button] == (uint8_t)INPUT_STATE::DOWN;
}

bool Input::IsMouseButtonUp(Window* window, MOUSE_BUTTON button)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_mouseButtonStates.find(glfwWin) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[glfwWin].find((uint8_t)button) == _mouseButtonStates[glfwWin].end())
		return true;
	INPUT_STATE currState = (INPUT_STATE)_mouseButtonStates[glfwWin][(uint8_t)button];
	return currState == INPUT_STATE::UP || currState == INPUT_STATE::RELEASED;
}

bool Input::IsMouseButtonHeldUp(Window* window, MOUSE_BUTTON button)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_mouseButtonStates.find(glfwWin) == _mouseButtonStates.end())
		return false;
	if (_mouseButtonStates[glfwWin].find((uint8_t)button) == _mouseButtonStates[glfwWin].end())
		return true;
	return _mouseButtonStates[glfwWin][(uint8_t)button] == (uint8_t)INPUT_STATE::UP;
}

EventHandler<MOUSE_BUTTON, INPUT_STATE>& Input::GetOnMouseButtonStateChange(Window* window)
{
	return _onMouseButtonStateChange[window->GetWindow()];
}

bool Input::IsKeyPressed(Window* window, KEY key)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_keyStates.find(glfwWin) == _keyStates.end())
		return false;
	uint16_t keyStateCode = (uint16_t)key - (uint16_t)KEY::SPACE;
	if (_keyStates[glfwWin].find(keyStateCode) == _keyStates[glfwWin].end())
		return false;
	return _keyStates[glfwWin][keyStateCode] == (uint8_t)INPUT_STATE::PRESSED;
}

bool Input::IsKeyReleased(Window* window, KEY key)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_keyStates.find(glfwWin) == _keyStates.end())
		return false;
	uint16_t keyStateCode = (uint16_t)key - (uint16_t)KEY::SPACE;
	if (_keyStates[glfwWin].find(keyStateCode) == _keyStates[glfwWin].end())
		return false;
	return _keyStates[glfwWin][keyStateCode] == (uint8_t)INPUT_STATE::RELEASED;
}

bool Input::IsKeyDown(Window* window, KEY key)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_keyStates.find(glfwWin) == _keyStates.end())
		return false;
	uint16_t keyStateCode = (uint16_t)key - (uint16_t)KEY::SPACE;
	if (_keyStates[glfwWin].find(keyStateCode) == _keyStates[glfwWin].end())
		return false;
	INPUT_STATE currState = (INPUT_STATE)_keyStates[glfwWin][keyStateCode];
	return currState == INPUT_STATE::PRESSED || currState == INPUT_STATE::PRESSED_LONGER || currState == INPUT_STATE::DOWN;
}

bool Input::IsKeyHeldDown(Window* window, KEY key)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_keyStates.find(glfwWin) == _keyStates.end())
		return false;
	uint16_t keyStateCode = (uint16_t)key - (uint16_t)KEY::SPACE;
	if (_keyStates[glfwWin].find(keyStateCode) == _keyStates[glfwWin].end())
		return false;
	INPUT_STATE currState = (INPUT_STATE)_keyStates[glfwWin][keyStateCode];
	return currState == INPUT_STATE::DOWN;
}

bool Input::IsKeyUp(Window* window, KEY key)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_keyStates.find(glfwWin) == _keyStates.end())
		return false;
	uint16_t keyStateCode = (uint16_t)key - (uint16_t)KEY::SPACE;
	if (_keyStates[glfwWin].find(keyStateCode) == _keyStates[glfwWin].end())
		return true;
	INPUT_STATE currState = (INPUT_STATE)_keyStates[glfwWin][keyStateCode];
	return currState == INPUT_STATE::RELEASED || currState == INPUT_STATE::UP;
}

bool Input::IsKeyHeldUp(Window* window, KEY key)
{
	GLFWwindow* glfwWin = window->GetWindow();
	if (_keyStates.find(glfwWin) == _keyStates.end())
		return false;
	uint16_t keyStateCode = (uint16_t)key - (uint16_t)KEY::SPACE;
	if (_keyStates[glfwWin].find(keyStateCode) == _keyStates[glfwWin].end())
		return true;
	return _keyStates[glfwWin][keyStateCode] == (uint8_t)INPUT_STATE::UP;
}

EventHandler<KEY, INPUT_STATE, KEY_MOD>& Input::GetOnKeyStateChange(Graphic::Window* window)
{
	return _onKeyStateChange[window->GetWindow()];
}

EventHandler<unsigned int>& Input::GetOnTextInput(Window* window)
{
	return _onTextInput[window->GetWindow()];
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

vec2 Input::GetCursorPos()
{
	return GetCursorPos(_mainWindow);
}

EventHandler<vec2>& Input::GetOnCursorPosChange()
{
	return GetOnCursorPosChange(_mainWindow);
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

EventHandler<MOUSE_BUTTON, INPUT_STATE>& Input::GetOnMouseButtonStateChange()
{
	return GetOnMouseButtonStateChange(_mainWindow);
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

EventHandler<KEY, INPUT_STATE, KEY_MOD>& Input::GetOnKeyStateChange()
{
	return GetOnKeyStateChange(_mainWindow);
}

EventHandler<unsigned int>& Input::GetOnTextInput()
{
	return GetOnTextInput(_mainWindow);
}