#pragma once

#include <tools/EventHandler.h>

namespace Twin2Engine {
	namespace Graphic {
		class Window;
	}

	namespace Core {
		enum class INPUT_STATE {
			UP = 0,
			DOWN = 1,
			PRESSED = 2,
			PRESSED_LONGER = 3,
			RELEASED = 4
		};

		enum class CURSOR_STATE {
			DISABLED = GLFW_CURSOR_DISABLED,
			HIDDEN = GLFW_CURSOR_HIDDEN,
			CAPTURED = GLFW_CURSOR_CAPTURED,
			NORMAL = GLFW_CURSOR_NORMAL
		};

		enum class KEY {
			SPACE = GLFW_KEY_SPACE,
			APOSTROPHE = GLFW_KEY_APOSTROPHE,
			COMMA = GLFW_KEY_COMMA,
			MINUS = GLFW_KEY_MINUS,
			PERIOD = GLFW_KEY_PERIOD,
			SLASH = GLFW_KEY_SLASH,
			NUM_0 = GLFW_KEY_0,
			NUM_1 = GLFW_KEY_1,
			NUM_2 = GLFW_KEY_2,
			NUM_3 = GLFW_KEY_3,
			NUM_4 = GLFW_KEY_4,
			NUM_5 = GLFW_KEY_5,
			NUM_6 = GLFW_KEY_6,
			NUM_7 = GLFW_KEY_7,
			NUM_8 = GLFW_KEY_8,
			NUM_9 = GLFW_KEY_9,
			SEMICOLON = GLFW_KEY_SEMICOLON,
			EQUAL = GLFW_KEY_EQUAL,
			A = GLFW_KEY_A,
			B = GLFW_KEY_B,
			C = GLFW_KEY_C,
			D = GLFW_KEY_D,
			E = GLFW_KEY_E,
			F = GLFW_KEY_F,
			G = GLFW_KEY_G,
			H = GLFW_KEY_H,
			I = GLFW_KEY_I,
			J = GLFW_KEY_J,
			K = GLFW_KEY_K,
			L = GLFW_KEY_L,
			M = GLFW_KEY_M,
			N = GLFW_KEY_N,
			O = GLFW_KEY_O,
			P = GLFW_KEY_P,
			Q = GLFW_KEY_Q,
			R = GLFW_KEY_R,
			S = GLFW_KEY_S,
			T = GLFW_KEY_T,
			U = GLFW_KEY_U,
			V = GLFW_KEY_V,
			W = GLFW_KEY_W,
			X = GLFW_KEY_X,
			Y = GLFW_KEY_Y,
			Z = GLFW_KEY_Z,
			LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
			BACKSLASH = GLFW_KEY_BACKSLASH,
			RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
			GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
			WORLD_1 = GLFW_KEY_WORLD_1,
			WORLD_2 = GLFW_KEY_WORLD_2,
			ESCAPE = GLFW_KEY_ESCAPE,
			ENTER = GLFW_KEY_ENTER,
			TAB = GLFW_KEY_TAB,
			BACKSPACE = GLFW_KEY_BACKSPACE,
			INSERT = GLFW_KEY_INSERT,
			DELETE_KEY = GLFW_KEY_DELETE,
			ARROW_RIGHT = GLFW_KEY_RIGHT,
			ARROW_LEFT = GLFW_KEY_LEFT,
			ARROW_DOWN = GLFW_KEY_DOWN,
			ARROW_UP = GLFW_KEY_UP,
			PAGE_UP = GLFW_KEY_PAGE_UP,
			PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
			HOME = GLFW_KEY_HOME,
			END = GLFW_KEY_END,
			CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
			SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
			NUM_LOCK = GLFW_KEY_NUM_LOCK,
			PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
			PAUSE = GLFW_KEY_PAUSE,
			F1 = GLFW_KEY_F1,
			F2 = GLFW_KEY_F2,
			F3 = GLFW_KEY_F3,
			F4 = GLFW_KEY_F4,
			F5 = GLFW_KEY_F5,
			F6 = GLFW_KEY_F6,
			F7 = GLFW_KEY_F7,
			F8 = GLFW_KEY_F8,
			F9 = GLFW_KEY_F9,
			F10 = GLFW_KEY_F10,
			F11 = GLFW_KEY_F11,
			F12 = GLFW_KEY_F12,
			F13 = GLFW_KEY_F13,
			F14 = GLFW_KEY_F14,
			F15 = GLFW_KEY_F15,
			F16 = GLFW_KEY_F16,
			F17 = GLFW_KEY_F17,
			F18 = GLFW_KEY_F18,
			F19 = GLFW_KEY_F19,
			F20 = GLFW_KEY_F20,
			F21 = GLFW_KEY_F21,
			F22 = GLFW_KEY_F22,
			F23 = GLFW_KEY_F23,
			F24 = GLFW_KEY_F24,
			F25 = GLFW_KEY_F25,
			NUMPAD_0 = GLFW_KEY_KP_0,
			NUMPAD_1 = GLFW_KEY_KP_1,
			NUMPAD_2 = GLFW_KEY_KP_2,
			NUMPAD_3 = GLFW_KEY_KP_3,
			NUMPAD_4 = GLFW_KEY_KP_4,
			NUMPAD_5 = GLFW_KEY_KP_5,
			NUMPAD_6 = GLFW_KEY_KP_6,
			NUMPAD_7 = GLFW_KEY_KP_7,
			NUMPAD_8 = GLFW_KEY_KP_8,
			NUMPAD_9 = GLFW_KEY_KP_9,
			NUMPAD_DECIMAL = GLFW_KEY_KP_DECIMAL,
			NUMPAD_DIVIDE = GLFW_KEY_KP_DIVIDE,
			NUMPAD_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
			NUMPAD_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
			NUMPAD_ADD = GLFW_KEY_KP_ADD,
			NUMPAD_ENTER = GLFW_KEY_KP_ENTER,
			NUMPAD_EQUAL = GLFW_KEY_KP_EQUAL,
			LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
			LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
			LEFT_ALT = GLFW_KEY_LEFT_ALT,
			LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
			RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
			RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
			RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
			RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
			MENU = GLFW_KEY_MENU,
			KEYS_SIZE = GLFW_KEY_LAST + 1 - GLFW_KEY_SPACE,
		};

		enum class KEY_MOD {
			NONE = 0,
			SHIFT_MOD = GLFW_MOD_SHIFT,
			CTRL_MOD = GLFW_MOD_CONTROL,
			ALT_MOD = GLFW_MOD_ALT,
			SUPER_MOD = GLFW_MOD_SUPER,
			CAPS_LOCK_MOD = GLFW_MOD_CAPS_LOCK,
			NUM_LOCK_MOD = GLFW_MOD_NUM_LOCK
		};

		enum class MOUSE_BUTTON {
			NR_1 = GLFW_MOUSE_BUTTON_1,
			NR_2 = GLFW_MOUSE_BUTTON_2,
			NR_3 = GLFW_MOUSE_BUTTON_3,
			NR_4 = GLFW_MOUSE_BUTTON_4,
			NR_5 = GLFW_MOUSE_BUTTON_5,
			NR_6 = GLFW_MOUSE_BUTTON_6,
			NR_7 = GLFW_MOUSE_BUTTON_7,
			NR_8 = GLFW_MOUSE_BUTTON_8,
			LEFT = GLFW_MOUSE_BUTTON_LEFT,
			RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
			MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
			MOUSE_BUTTONS_SIZE = GLFW_MOUSE_BUTTON_LAST + 1
		};

		class Input {
		private:
			static Graphic::Window* _mainWindow;

			static std::vector<Graphic::Window*> _windows;
			static std::map<GLFWwindow*, std::map<uint8_t, uint8_t>> _mouseButtonStates;
			static std::map<GLFWwindow*, std::map<uint16_t, uint8_t>> _keyStates;

			static std::map<GLFWwindow*, Tools::EventHandler<KEY, INPUT_STATE, KEY_MOD>> _onKeyStateChange;
			static std::map<GLFWwindow*, Tools::EventHandler<unsigned int>> _onTextInput;
			static std::map<GLFWwindow*, Tools::EventHandler<glm::vec2>> _onCursorPosChange;
			static std::map<GLFWwindow*, Tools::EventHandler<MOUSE_BUTTON, INPUT_STATE>> _onMouseButtonStateChange;

			static void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
			static void text_input_callback(GLFWwindow* win, unsigned int codepoint);
			static void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
			static void cursor_pos_callback(GLFWwindow* win, double xpos, double ypos);

			static bool IsInizializedForWindow(Graphic::Window* window);
			static Graphic::Window* GetWindow(GLFWwindow* window);
		public:
			static void InitForWindow(Graphic::Window* window, bool mainWindow = false);
			static void FreeWindow(Graphic::Window* window);
			static void FreeAllWindows();
			static void SetMainWindow(Graphic::Window* window);
			static Graphic::Window* GetMainWindow();
			static void Update();

			static std::string GetKeyName(KEY key);

#pragma region WITH_WINDOW_CLASS
			// CURSOR
			static void HideAndLockCursor(Graphic::Window* window);
			static void HideCursor(Graphic::Window* window);
			static void KeepCursorInWindow(Graphic::Window* window);
			static void ShowCursor(Graphic::Window* window);
			static CURSOR_STATE GetCursorState(Graphic::Window* window);
			static glm::vec2 GetCursorPos(Graphic::Window* window);
			static Tools::EventHandler<glm::vec2>& GetOnCursorPosChange(Graphic::Window* window);

			// MOUSE
			static bool IsMouseButtonPressed(Graphic::Window* window, MOUSE_BUTTON button);
			static bool IsMouseButtonReleased(Graphic::Window* window, MOUSE_BUTTON button);
			static bool IsMouseButtonDown(Graphic::Window* window, MOUSE_BUTTON button);
			static bool IsMouseButtonHeldDown(Graphic::Window* window, MOUSE_BUTTON button);
			static bool IsMouseButtonUp(Graphic::Window* window, MOUSE_BUTTON button);
			static bool IsMouseButtonHeldUp(Graphic::Window* window, MOUSE_BUTTON button);
			static Tools::EventHandler<MOUSE_BUTTON, INPUT_STATE>& GetOnMouseButtonStateChange(Graphic::Window* window);

			// KEYS
			static bool IsKeyPressed(Graphic::Window* window, KEY key);
			static bool IsKeyReleased(Graphic::Window* window, KEY key);
			static bool IsKeyDown(Graphic::Window* window, KEY key);
			static bool IsKeyHeldDown(Graphic::Window* window, KEY key);
			static bool IsKeyUp(Graphic::Window* window, KEY key);
			static bool IsKeyHeldUp(Graphic::Window* window, KEY key);
			static Tools::EventHandler<KEY, INPUT_STATE, KEY_MOD>& GetOnKeyStateChange(Graphic::Window* window);
			static Tools::EventHandler<unsigned int>& GetOnTextInput(Graphic::Window* window);
#pragma endregion

#pragma region WITH_MAIN_WINDOW
			// CURSOR
			static void HideAndLockCursor();
			static void HideCursor();
			static void KeepCursorInWindow();
			static void ShowCursor();
			static CURSOR_STATE GetCursorState();
			static glm::vec2 GetCursorPos();
			static Tools::EventHandler<glm::vec2>& GetOnCursorPosChange();

			// MOUSE
			static bool IsMouseButtonPressed(MOUSE_BUTTON button);
			static bool IsMouseButtonReleased(MOUSE_BUTTON button);
			static bool IsMouseButtonDown(MOUSE_BUTTON button);
			static bool IsMouseButtonHeldDown(MOUSE_BUTTON button);
			static bool IsMouseButtonUp(MOUSE_BUTTON button);
			static bool IsMouseButtonHeldUp(MOUSE_BUTTON button);
			static Tools::EventHandler<MOUSE_BUTTON, INPUT_STATE>& GetOnMouseButtonStateChange();

			// KEY
			static bool IsKeyPressed(KEY key);
			static bool IsKeyReleased(KEY key);
			static bool IsKeyDown(KEY key);
			static bool IsKeyHeldDown(KEY key);
			static bool IsKeyUp(KEY key);
			static bool IsKeyHeldUp(KEY key);
			static Tools::EventHandler<KEY, INPUT_STATE, KEY_MOD>& GetOnKeyStateChange();
			static Tools::EventHandler<unsigned int>& GetOnTextInput();
#pragma endregion
		};
	}
}