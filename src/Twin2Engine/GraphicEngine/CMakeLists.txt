project(GraphicEngine VERSION 1.0)

set(GRAPHIC_ENGINE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(GRAPHIC_ENGINE_SOURCE_DIR ${GRAPHIC_ENGINE_DIR}/src)
set(GRAPHIC_ENGINE_INCLUDE_DIR ${GRAPHIC_ENGINE_DIR}/include)
set(GRAPHIC_ENGINE_PRECOMPILE_HEADER ${GRAPHIC_ENGINE_DIR}/pch.h)

file(GLOB_RECURSE GRAPHIC_ENGINE_SOURCE_FILES 
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.cpp
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES 
                        ${GRAPHIC_ENGINE_INCLUDE_DIR}/*.h
                        ${GRAPHIC_ENGINE_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE GRAPHIC_ENGINE_PRIVATE_INCLUDE_FILES  
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.h
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GRAPHIC_ENGINE_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GRAPHIC_ENGINE_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${GRAPHIC_ENGINE_SOURCE_FILES})

# Adding include to library
target_include_directories(${PROJECT_NAME} PUBLIC	${GRAPHIC_ENGINE_INCLUDE_DIR}
													${PHYSIC_ENGINE_INCLUDE_DIR}
													${TOOLS_INCLUDE_DIR}
													${glad_SOURCE_DIR}
													${stb_image_SOURCE_DIR})

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${GRAPHIC_ENGINE_PRECOMPILE_HEADER})

target_link_libraries(${PROJECT_NAME}	${OPENGL_LIBRARIES}
										glad
										stb_image
										assimp
										glfw
										spdlog
										yaml-cpp
										freetype
										glm::glm
										PhysicEngine
										Tools)