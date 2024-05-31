project(EnemyAI VERSION 1.0)


set(ENEMY_AI_DIR ${CMAKE_CURRENT_LIST_DIR})
set(ENEMY_AI_SOURCE_DIR ${ENEMY_AI_DIR}/src)
set(ENEMY_AI_INCLUDE_DIR ${ENEMY_AI_DIR}/include)
set(ENEMY_AI_PRECOMPILE_HEADER ${ENEMY_AI_DIR}/pch.h)

file(GLOB_RECURSE ENEMY_AI_SOURCE_FILES 
                        ${ENEMY_AI_SOURCE_DIR}/*.cpp
                        ${ENEMY_AI_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE ENEMY_AI_PUBLIC_INCLUDE_FILES 
                        ${ENEMY_AI_INCLUDE_DIR}/*.h
                        ${ENEMY_AI_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE ENEMY_AI_PRIVATE_INCLUDE_FILES  
                        ${ENEMY_AI_SOURCE_DIR}/*.h
                        ${ENEMY_AI_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${ENEMY_AI_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${ENEMY_AI_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${ENEMY_AI_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${ENEMY_AI_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${ENEMY_AI_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${ENEMY_AI_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PUBLIC Twin2Engine)
target_link_libraries(${PROJECT_NAME} PUBLIC StateMachine)
target_link_libraries(${PROJECT_NAME} PUBLIC DecisionTree)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)

if (TRACY_PROFILER)
    target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	 target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
	 target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
else()
    target_compile_options(${PROJECT_NAME} PUBLIC "-O2")
endif()