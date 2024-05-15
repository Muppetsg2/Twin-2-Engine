#pragma once

// STANDARD LIBRARIES
#include <unordered_map>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <set>
#include <queue>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// YAML
#include <yaml-cpp/yaml.h>

// SPDLOG
#include <spdlog/spdlog.h>

// STB_IMAGE
#include <stb_image.h>



#if _DEBUG
	#include <tracy/Tracy.hpp>
	#include <tracy/TracyOpenGL.hpp>
	#define TRACY_ENABLE
#endif