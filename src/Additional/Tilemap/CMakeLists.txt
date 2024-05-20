project(Tilemap VERSION 1.0)


set(TILEMAP_DIR ${CMAKE_CURRENT_LIST_DIR})
set(TILEMAP_SOURCE_DIR ${TILEMAP_DIR}/src)
set(TILEMAP_INCLUDE_DIR ${TILEMAP_DIR}/include)
set(TILEMAP_PRECOMPILE_HEADER ${TILEMAP_DIR}/pch.h)

file(GLOB_RECURSE TILEMAP_SOURCE_FILES 
                        ${TILEMAP_SOURCE_DIR}/*.cpp
                        ${TILEMAP_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE TILEMAP_PUBLIC_INCLUDE_FILES 
                        ${TILEMAP_INCLUDE_DIR}/*.h
                        ${TILEMAP_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE TILEMAP_PRIVATE_INCLUDE_FILES  
                        ${TILEMAP_SOURCE_DIR}/*.h
                        ${TILEMAP_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${TILEMAP_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${TILEMAP_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${TILEMAP_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${TILEMAP_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${TILEMAP_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${TILEMAP_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PUBLIC Twin2Engine)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC Generation)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
	 target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
	 target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
else()
    target_compile_options(${PROJECT_NAME} PUBLIC "-O2")
endif()