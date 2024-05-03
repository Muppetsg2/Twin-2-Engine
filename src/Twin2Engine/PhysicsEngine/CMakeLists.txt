project(PhysicsEngine VERSION 1.0)

set(PHYSICS_ENGINE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(PHYSICS_ENGINE_SOURCE_DIR ${PHYSICS_ENGINE_DIR}/src)
set(PHYSICS_ENGINE_INCLUDE_DIR ${PHYSICS_ENGINE_DIR}/include)
set(PHYSICS_ENGINE_PRECOMPILE_HEADER ${PHYSICS_ENGINE_DIR}/pch.h)

file(GLOB_RECURSE PHYSICS_ENGINE_SOURCE_FILES 
                        ${PHYSICS_ENGINE_SOURCE_DIR}/*.cpp
                        ${PHYSICS_ENGINE_SOURCE_DIR}/*.c)


file(GLOB_RECURSE PHYSICS_ENGINE_PUBLIC_INCLUDE_FILES 
                        ${PHYSICS_ENGINE_INCLUDE_DIR}/*.h
                        ${PHYSICS_ENGINE_INCLUDE_DIR}/*.hpp)

file(GLOB_RECURSE PHYSICS_ENGINE_PRIVATE_INCLUDE_FILES
                        ${PHYSICS_ENGINE_SOURCE_DIR}/*.h
                        ${PHYSICS_ENGINE_SOURCE_DIR}/*.hpp)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PHYSICS_ENGINE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PHYSICS_ENGINE_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PHYSICS_ENGINE_PRIVATE_INCLUDE_FILES})


#Defining library
add_library(${PROJECT_NAME} STATIC ${PHYSICS_ENGINE_SOURCE_FILES})

#Adding precompile header
target_precompile_headers(${PROJECT_NAME} PUBLIC ${PHYSICS_ENGINE_PRECOMPILE_HEADER})

#Adding include to library
target_include_directories(${PROJECT_NAME} PUBLIC ${PHYSICS_ENGINE_INCLUDE_DIR}
                                                  ${GRAPHIC_ENGINE_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog
                                              glm::glm
                                              GraphicEngine
                                              Tools)