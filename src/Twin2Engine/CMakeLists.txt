project(Twin2Engine VERSION 1.0)

set(Twin2Engine_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(Twin2Engine_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(Twin2Engine_PRECOMPILE_HEADERS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	 ${Twin2Engine_SOURCE_DIR}/*.c
	 ${Twin2Engine_SOURCE_DIR}/*.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES
	 ${Twin2Engine_HEADER_DIR}/*.h
	 ${Twin2Engine_HEADER_DIR}/*.hpp)
	 
# Search for the assets files
file(GLOB_RECURSE ASSETS_FILES 
	 ${CMAKE_SOURCE_DIR}/res/*.*)

set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}         FILES ${ASSETS_FILES})


add_subdirectory(GraphicEngine)
add_subdirectory(CollisionSystem)
add_subdirectory(LightingSystem)

# ---- Dependency ----
add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

get_property(is_miniaudio GLOBAL PROPERTY ISMINIAUDIO)
get_property(is_imgui GLOBAL PROPERTY USEIMGUI)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${Twin2Engine_PRECOMPILE_HEADERS_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC ${Twin2Engine_HEADER_DIR}
												  ${GRAPHIC_ENGINE_INCLUDE_DIR}
												  ${COLLISION_SYSTEM_INCLUDE_DIR}
												  ${LIGHTING_SYSTEM_INCLUDE_DIR}
												  ${glad_SOURCE_DIR}
												  ${stb_image_SOURCE_DIR}
												  ${freetype_SOURCE_DIR})

if (is_miniaudio)
	target_include_directories(${PROJECT_NAME} PUBLIC ${miniaudio_SOURCE_DIR})
	target_link_libraries(${PROJECT_NAME} miniaudio)
else()
	target_include_directories(${PROJECT_NAME} PUBLIC ${soloud_SOURCE_DIR})
	target_link_libraries(${PROJECT_NAME} soloud)
endif()

if (is_imgui)
	target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
	target_link_libraries(${PROJECT_NAME} imgui)
endif()

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} stb_image)
#target_link_libraries(${PROJECT_NAME} assimp)
#target_link_libraries(${PROJECT_NAME} tinygltf)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} spdlog)
target_link_libraries(${PROJECT_NAME} yaml-cpp)
target_link_libraries(${PROJECT_NAME} freetype)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} GraphicEngine)
target_link_libraries(${PROJECT_NAME} CollisionSystem)
target_link_libraries(${PROJECT_NAME} LightingSystem)