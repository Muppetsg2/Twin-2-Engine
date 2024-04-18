project(LightingSystem VERSION 1.0)

set(LIGHTING_SYSTEM_DIR ${CMAKE_CURRENT_LIST_DIR})
set(LIGHTING_SYSTEM_SOURCE_DIR ${LIGHTING_SYSTEM_DIR}/src)
set(LIGHTING_SYSTEM_INCLUDE_DIR ${LIGHTING_SYSTEM_DIR}/include)
set(LIGHTING_SYSTEM_PRECOMPILE_HEADER ${LIGHTING_SYSTEM_DIR}/pch.h)

file(GLOB_RECURSE LIGHTING_SYSTEM_SOURCE_FILES 
                        ${LIGHTING_SYSTEM_SOURCE_DIR}/*.cpp
                        ${LIGHTING_SYSTEM_SOURCE_DIR}/*.c)


file(GLOB_RECURSE LIGHTING_SYSTEM_PUBLIC_INCLUDE_FILES 
                        ${LIGHTING_SYSTEM_INCLUDE_DIR}/*.h
                        ${LIGHTING_SYSTEM_INCLUDE_DIR}/*.hpp)

file(GLOB_RECURSE LIGHTING_SYSTEM_PRIVATE_INCLUDE_FILES
                        ${LIGHTING_SYSTEM_SOURCE_DIR}/*.h
                        ${LIGHTING_SYSTEM_SOURCE_DIR}/*.hpp)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${LIGHTING_SYSTEM_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${LIGHTING_SYSTEM_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${LIGHTING_SYSTEM_PRIVATE_INCLUDE_FILES})


#Defining library
add_library(${PROJECT_NAME} STATIC ${LIGHTING_SYSTEM_SOURCE_FILES})

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${LIGHTING_SYSTEM_PRECOMPILE_HEADER})

#Adding include to library
target_include_directories(${PROJECT_NAME}
                        PUBLIC 
                        ${LIGHTING_SYSTEM_INCLUDE_DIR}
                        ${TOOLS_INCLUDE_DIR}
                        ${glad_SOURCE_DIR}

                        PRIVATE
                        ${LIGHTING_SYSTEM_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE 
                                        glad
                                        glfw
                                        glm::glm
                                        GraphicEngine
                                        Tools
                                        spdlog)