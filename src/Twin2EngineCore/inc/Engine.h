#pragma once

// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>  // Initialize with gladLoadGL()
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

// Soloud
#include <soloud.h>
#include <soloud_wav.h>

/*
// Miniaudio
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
*/

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// HID
#include <inc/Input.h>

constexpr int32_t WINDOW_WIDTH = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
const char* WINDOW_NAME = "Twin^2 Engine";
constexpr bool fullscreen = false;

GLFWmonitor* monitor = nullptr;
GLFWwindow* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char* glsl_version = "#version 450";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 5;

ImVec4 clear_color = ImVec4(.1f, .1f, .1f, 1.f);

GLuint UBOMatrices;

SoLoud::Soloud soloud;
SoLoud::Wav sample;
SoLoud::handle sampleHandle = 0;
bool first = true;

/*
ma_engine engine;
ma_sound sound;
*/

bool musicPlaying = false;

namespace Twin2EngineCore {
	class Twin2Engine {
	private:
		bool Init();
		void Init_Imgui();

		void Input();
		void Update();
		void Render();

		void Imgui_Begin();
		void Imgui_Render();
		void Imgui_End();

		void End_Frame();

	public:
		void GameLoop();
	};
}