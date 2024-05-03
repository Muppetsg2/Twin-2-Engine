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

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${TOOLS_PRECOMPILE_HEADER})

#Adding include to library
target_include_directories(${PROJECT_NAME}
                        PUBLIC 
                        ${TOOLS_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog
                                              glm::glm)