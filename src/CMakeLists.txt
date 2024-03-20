# Add source files
#file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_SOURCE_DIR}/ShadersOrigin SYMBOLIC)
file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)
configure_file(GameConfig.yaml.in GameConfig.yaml)

file(GLOB_RECURSE SOURCE_FILES 
	 *.c
	 *.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	 *.h
	 *.hpp)
	 
# Search for the assets files
file(GLOB_RECURSE ASSETS_FILES 
	 ${CMAKE_SOURCE_DIR}/res/*.*)

set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}         FILES ${ASSETS_FILES})

add_subdirectory(GraphicEngine)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES} )

target_link_libraries(${PROJECT_NAME} PRIVATE GraphicEngine)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
												  ${glad_SOURCE_DIR}
												  ${stb_image_SOURCE_DIR}
												  ${soloud_SOURCE_DIR}
												  ${imgui_SOURCE_DIR}
												  ${GRAPHIC_ENGINE_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE stb_image)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE soloud)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
				   COMMAND ${CMAKE_COMMAND} -E create_symlink 
				   ${CMAKE_SOURCE_DIR}/res 
				   ${CMAKE_CURRENT_BINARY_DIR}/res)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif()
