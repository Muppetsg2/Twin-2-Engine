#pragma once

#include <memory>

//LOGGER
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/common.h>
#include <spdlog/details/console_globals.h>
#include <spdlog/details/null_mutex.h>

// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if TRACY_PROFILER
	#include <tracy/Tracy.hpp>
	#include <tracy/TracyOpenGL.hpp>
	#define TRACY_ENABLE
#endif

#if _DEBUG
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include <imgui_impl/imgui_user.h>
	#include <imgui_impl/imgui_filedialog.h>
	#include <imgui.h>
	#include <imgui_impl/imgui_stdlib.h>
	#include <imgui_impl/imgui_impl_glfw.h>
	#include <imgui_impl/imgui_impl_opengl3.h>
#endif

#include <stb_image.h>

#include <yaml-cpp/node/node.h>
