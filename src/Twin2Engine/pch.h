#pragma once

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl/imgui_impl_glfw.h>
#include <imgui_impl/imgui_impl_opengl3.h>

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
#include <memory>

// YAML
#include <yaml-cpp/yaml.h>

// MACROS
#include "SerializationMacros.h"