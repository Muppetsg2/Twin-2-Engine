#pragma once

// STD
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


#include <mutex>

#include <windows.h>


// YAML-CPP
#include <yaml-cpp/yaml.h>


//LOGGER
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/base_sink-inl.h>
//#include <spdlog/sinks/stdout_color_sinks.h>
//#include <spdlog/sinks/wincolor_sink.h>
//#include <spdlog/details/synchronous_factory.h>


// GLM
#include <glm/glm.hpp>


// ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>