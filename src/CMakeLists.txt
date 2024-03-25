
#file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)
configure_file(GameConfig.yaml.in GameConfig.yaml)

file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)
file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_SOURCE_DIR}/ShadersOrigin SYMBOLIC)

# ---- Dependency ----
add_subdirectory(Twin2Engine)

#add_subdirectory(GraphicEngine)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${Twin2Engine_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Twin2Engine)

#target_include_directories(${PROJECT_NAME} PUBLIC ${GRAPHIC_ENGINE_INCLUDE_DIR})
#target_link_libraries(${PROJECT_NAME} PRIVATE GraphicEngine)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
				   COMMAND ${CMAKE_COMMAND} -E create_symlink 
				   ${CMAKE_SOURCE_DIR}/res 
				   ${CMAKE_CURRENT_BINARY_DIR}/res)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif()
