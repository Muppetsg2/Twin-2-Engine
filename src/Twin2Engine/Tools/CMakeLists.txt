project(Tools VERSION 1.0)

set(TOOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOOLS_SOURCE_DIR ${TOOLS_DIR}/src)
set(TOOLS_INCLUDE_DIR ${TOOLS_DIR}/include)
set(TOOLS_PRECOMPILE_HEADER ${TOOLS_DIR}/pch.h)

file(GLOB_RECURSE TOOLS_SOURCE_FILES 
                        ${TOOLS_SOURCE_DIR}/*.cpp
                        ${TOOLS_SOURCE_DIR}/*.c)


file(GLOB_RECURSE TOOLS_PUBLIC_INCLUDE_FILES 
                        ${TOOLS_INCLUDE_DIR}/*.h
                        ${TOOLS_INCLUDE_DIR}/*.hpp)

file(GLOB_RECURSE TOOLS_PRIVATE_INCLUDE_FILES
                        ${TOOLS_SOURCE_DIR}/*.h
                        ${TOOLS_SOURCE_DIR}/*.hpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TOOLS_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TOOLS_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TOOLS_PRIVATE_INCLUDE_FILES})


#Defining library
add_library(${PROJECT_NAME} STATIC ${TOOLS_SOURCE_FILES} ${TOOLS_INCLUDE_DIR})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${TOOLS_PRECOMPILE_HEADER})

#Adding include to library
target_include_directories(${PROJECT_NAME}
                        PUBLIC 
                        ${TOOLS_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog
                                             glm::glm
                                             GraphicEngine)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
	 target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
	 target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
endif()