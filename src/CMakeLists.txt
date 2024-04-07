
#file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)
configure_file(GameConfig.yaml.in GameConfig.yaml)

file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)
file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_SOURCE_DIR}/ShadersOrigin SYMBOLIC)

# ---- Dependency ----
add_subdirectory(Twin2Engine)

add_subdirectory(CollisionSystem)
add_subdirectory(LightingSystem)

add_executable(${PROJECT_NAME} main.cpp)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${Twin2Engine_HEADER_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Twin2Engine)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

target_include_directories(${PROJECT_NAME} PUBLIC ${COLLISION_SYSTEM_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC CollisionSystem)

target_include_directories(${PROJECT_NAME} PUBLIC ${LIGHTING_SYSTEM_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC LightingSystem)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
				   COMMAND ${CMAKE_COMMAND} -E create_symlink 
				   ${CMAKE_SOURCE_DIR}/res 
				   ${CMAKE_CURRENT_BINARY_DIR}/res)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif()
