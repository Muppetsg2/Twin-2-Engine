project(StateMachine VERSION 1.0)


set(STATEMACHINE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(STATEMACHINE_SOURCE_DIR ${STATEMACHINE_DIR}/src)
set(STATEMACHINE_INCLUDE_DIR ${STATEMACHINE_DIR}/include)
set(STATEMACHINE_PRECOMPILE_HEADER ${STATEMACHINE_DIR}/pch.h)

file(GLOB_RECURSE STATEMACHINE_SOURCE_FILES 
                        ${STATEMACHINE_SOURCE_DIR}/*.cpp
                        ${STATEMACHINE_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE STATEMACHINE_PUBLIC_INCLUDE_FILES 
                        ${STATEMACHINE_INCLUDE_DIR}/*.h
                        ${STATEMACHINE_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE STATEMACHINE_PRIVATE_INCLUDE_FILES  
                        ${STATEMACHINE_SOURCE_DIR}/*.h
                        ${STATEMACHINE_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${STATEMACHINE_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${STATEMACHINE_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${STATEMACHINE_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${STATEMACHINE_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${STATEMACHINE_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${TILEMAP_PRECOMPILE_HEADER})


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