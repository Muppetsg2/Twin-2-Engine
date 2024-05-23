project(GameScripts VERSION 1.0)

set(GAMESCRIPTS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(GAMESCRIPTS_SOURCE_DIR ${GAMESCRIPTS_DIR}/src)
set(GAMESCRIPTS_INCLUDE_DIR ${GAMESCRIPTS_DIR}/include)
set(GAMESCRIPTS_PRECOMPILE_HEADER ${GAMESCRIPTS_DIR}/pch.h)

file(GLOB_RECURSE GAMESCRIPTS_SOURCE_FILES 
                        ${GAMESCRIPTS_SOURCE_DIR}/*.cpp
                        ${GAMESCRIPTS_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE GAMESCRIPTS_PUBLIC_INCLUDE_FILES 
                        ${GAMESCRIPTS_INCLUDE_DIR}/*.h
                        ${GAMESCRIPTS_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE GAMESCRIPTS_PRIVATE_INCLUDE_FILES  
                        ${GAMESCRIPTS_SOURCE_DIR}/*.h
                        ${GAMESCRIPTS_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GAMESCRIPTS_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GAMESCRIPTS_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GAMESCRIPTS_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${GAMESCRIPTS_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${GAMESCRIPTS_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${GAMESCRIPTS_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PRIVATE Twin2Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE Tilemap)
target_link_libraries(${PROJECT_NAME} PRIVATE Tools)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE EngineAlgorithms)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Generation)

if (TRACY_PROFILER)
    target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	 target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
	 target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
else()
    target_compile_options(${PROJECT_NAME} PUBLIC "-O2")
endif()
