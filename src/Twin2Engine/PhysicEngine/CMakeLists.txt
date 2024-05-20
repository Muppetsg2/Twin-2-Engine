project(PhysicEngine VERSION 1.0)

set(PHYSIC_ENGINE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(PHYSIC_ENGINE_SOURCE_DIR ${PHYSIC_ENGINE_DIR}/src)
set(PHYSIC_ENGINE_INCLUDE_DIR ${PHYSIC_ENGINE_DIR}/include)
set(PHYSIC_ENGINE_PRECOMPILE_HEADER ${PHYSIC_ENGINE_DIR}/pch.h)

file(GLOB_RECURSE PHYSIC_ENGINE_SOURCE_FILES 
                        ${PHYSIC_ENGINE_SOURCE_DIR}/*.cpp
                        ${PHYSIC_ENGINE_SOURCE_DIR}/*.c)


file(GLOB_RECURSE PHYSIC_ENGINE_PUBLIC_INCLUDE_FILES 
                        ${PHYSIC_ENGINE_INCLUDE_DIR}/*.h
                        ${PHYSIC_ENGINE_INCLUDE_DIR}/*.hpp)

file(GLOB_RECURSE PHYSIC_ENGINE_PRIVATE_INCLUDE_FILES
                        ${PHYSIC_ENGINE_SOURCE_DIR}/*.h
                        ${PHYSIC_ENGINE_SOURCE_DIR}/*.hpp)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PHYSIC_ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PHYSIC_ENGINE_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PHYSIC_ENGINE_PRIVATE_INCLUDE_FILES})


#Defining library
add_library(${PROJECT_NAME} STATIC ${PHYSIC_ENGINE_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

#Adding precompile header
target_precompile_headers(${PROJECT_NAME} PUBLIC ${PHYSIC_ENGINE_PRECOMPILE_HEADER})

#Adding include to library
target_include_directories(${PROJECT_NAME} PUBLIC ${PHYSIC_ENGINE_INCLUDE_DIR}
                                                  ${GRAPHIC_ENGINE_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog
                                              glm::glm
                                              GraphicEngine
                                              Tools)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
	 target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
	 target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
else()
    target_compile_options(${PROJECT_NAME} PUBLIC "-O2")
endif()