#pragma once

#include <functional>
#include <queue>
#include <vector>
#include <string>
#include <map>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/base_sink-inl.h>

// GLAD
#include <glad/glad.h>

#if TRACY_PROFILER
	#include <tracy/Tracy.hpp>
	#include <tracy/TracyOpenGL.hpp>
	#define TRACY_ENABLE
#endif