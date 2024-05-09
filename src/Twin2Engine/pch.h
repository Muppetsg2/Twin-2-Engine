#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if _DEBUG
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include <imgui_impl/imgui_user.h>
	#include <imgui_impl/imgui_filedialog.h>
	#include <imgui.h>
	#include <imgui_impl/imgui_impl_glfw.h>
	#include <imgui_impl/imgui_impl_opengl3.h>
#endif

#include <stb_image.h>

// LOGGER
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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

// STANDARD LIBRARY
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <functional>
#include <iostream>
#include <fstream>
#include <format>
#include <memory>
#include <random>
#include <type_traits>

// YAML
#include <yaml-cpp/yaml.h>

// MACROS
#include "SerializationMacros.h"



#if _DEBUG
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui_impl/imgui_user.h>
#include <imgui_impl/imgui_filedialog.h>
#include <imgui.h>
#include <imgui_impl/imgui_impl_glfw.h>
#include <imgui_impl/imgui_impl_opengl3.h>
#endif

#include <type_traits>