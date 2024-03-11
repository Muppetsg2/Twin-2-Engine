#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#define KEYS_MAX_NUM 349
#define MOUSE_BUTTONS_MAX_NUM 8

class Input {
private:
	static GLFWwindow* _mainWindow;
	
	static std::map<GLFWwindow*, bool*> _mouseButtonPressed;
	static std::map<GLFWwindow*, bool*> _mouseButtonHolded;

	static std::map<GLFWwindow*, bool*> _keyPressed;
	static std::map<GLFWwindow*, bool*> _keyHolded;

	static void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
public:
	static void initForWindow(GLFWwindow* window, bool mainWindow = false);
	static void freeWindow(GLFWwindow* window);
	static constexpr void setMainWindow(GLFWwindow* window);
	static constexpr GLFWwindow* getMainWindow();
	
#pragma region WITHOUT_MAIN_WINDOW
	static void hideAndLockCursor(GLFWwindow* window);
	static void hideCursor(GLFWwindow* window);
	static void keepCursorInWindow(GLFWwindow* window);
	static void showCursor(GLFWwindow* window);

	static glm::vec2 getMousePos(GLFWwindow* window);
	static bool isMouseButtonPressed(GLFWwindow* window, int button);
	static bool isMouseButtonReleased(GLFWwindow* window, int button);
	static bool isMouseButtonHold(GLFWwindow* window, int button);

	static bool isKeyPressed(GLFWwindow* window, int key);
	static bool isKeyReleased(GLFWwindow* window, int key);
	static bool isKeyHold(GLFWwindow* window, int key);
#pragma endregion

#pragma region WITH_MAIN_WINDOW
	static void hideAndLockCursor();
	static void hideCursor();
	static void keepCursorInWindow();
	static void showCursor();

	static glm::vec2 getMousePos();
	static bool isMouseButtonPressed(int button);
	static bool isMouseButtonReleased(int button);
	static bool isMouseButtonHold(int button);

	static bool isKeyPressed(int key);
	static bool isKeyReleased(int key);
	static bool isKeyHold(int key);
#pragma endregion
};