# MATERIAL_CREATOR

project(MaterialCreator)

file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_SOURCE_DIR}/bin/ShadersOrigin SYMBOLIC)
file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_SOURCE_DIR}/ShadersOrigin SYMBOLIC)

add_executable(${PROJECT_NAME} MaterialCreator.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)

#set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/MaterialCreator)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#message("Destination: ${CMAKE_BINARY_DIR}/bin")