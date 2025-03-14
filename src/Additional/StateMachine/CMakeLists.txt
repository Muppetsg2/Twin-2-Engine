project(StateMachine VERSION 1.0)


set(STATE_MACHINE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(STATE_MACHINE_SOURCE_DIR ${STATE_MACHINE_DIR}/src)
set(STATE_MACHINE_INCLUDE_DIR ${STATE_MACHINE_DIR}/include)
set(STATE_MACHINE_PRECOMPILE_HEADER ${STATE_MACHINE_DIR}/pch.h)

file(GLOB_RECURSE STATE_MACHINE_SOURCE_FILES 
                        ${STATE_MACHINE_SOURCE_DIR}/*.cpp
                        ${STATE_MACHINE_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE STATE_MACHINE_PUBLIC_INCLUDE_FILES 
                        ${STATE_MACHINE_INCLUDE_DIR}/*.h
                        ${STATE_MACHINE_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE STATE_MACHINE_PRIVATE_INCLUDE_FILES  
                        ${STATE_MACHINE_SOURCE_DIR}/*.h
                        ${STATE_MACHINE_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${STATE_MACHINE_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${STATE_MACHINE_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${STATE_MACHINE_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${STATE_MACHINE_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${STATE_MACHINE_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${STATE_MACHINE_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PUBLIC Twin2Engine)
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