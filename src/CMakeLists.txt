
#file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)
configure_file(GameConfig.yaml.in GameConfig.yaml)

# ---- Dependency ----
add_subdirectory(Twin2Engine)

add_subdirectory(GraphicEngine)

## Define the executable
#<<<<<<< HEAD
#add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES} )
#
#
#target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
#target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")
#
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#												  ${glad_SOURCE_DIR}
#												  ${stb_image_SOURCE_DIR}
#												  ${soloud_SOURCE_DIR}
#												  ${imgui_SOURCE_DIR}
#												  ${GRAPHIC_ENGINE_INCLUDE_DIR}
#)
#
#target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} PRIVATE glad)
#target_link_libraries(${PROJECT_NAME} PRIVATE stb_image)
#target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
#target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
#target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
#target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
#target_link_libraries(${PROJECT_NAME} PRIVATE soloud)
#target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
#target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
#=======
add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${GRAPHIC_ENGINE_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${Twin2Engine_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE GraphicEngine)
target_link_libraries(${PROJECT_NAME} PUBLIC Twin2Engine)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
				   COMMAND ${CMAKE_COMMAND} -E create_symlink 
				   ${CMAKE_SOURCE_DIR}/res 
				   ${CMAKE_CURRENT_BINARY_DIR}/res)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif()
