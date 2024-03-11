#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#define KEYS_MAX_NUM 349;

class Input {
private:
	static GLFWwindow* _mainWindow;
	static std::map<GLFWwindow*, bool[]> _pressed;
	static std::map<GLFWwindow*, bool[]> _holded;

	static void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
public:
	static void InitForWindow(const GLFWwindow* window, bool mainWindow = false);
	static void SetMainWindow(const GLFWwindow* window);

	static void 
	
#pragma region WITHOUT_MAIN_WINDOW
	static glm::vec2 getMousePos(const GLFWwindow* window) const;
	static bool getMouseButtonPressed(const GLFWwindow* window, int button) const;
	static bool getMouseButtonReleased(const GLFWwindow* window, int button) const;
	static bool getMouseButtonHold(const GLFWwindow* window, int button) const;

	static bool getKeyPressed(const GLFWwindow* window, int key) const;
	static bool getKeyReleased(const GLFWwindow* window, int key) const;
	static bool getKeyHold(const GLFWwindow* window, int key) const;
#pragma endregion

#pragma region WITH_MAIN_WINDOW
	static glm::vec2 getMousePos() const;
	static bool getMouseButtonPressed(int button) const;
	static bool getMouseButtonReleased(int button) const;
	static bool getMouseButtonHold(int button) const;

	static bool getKeyPressed(int key) const;
	static bool getKeyReleased(int key) const;
	static bool getKeyHold(int key) const;
#pragma endregion
};