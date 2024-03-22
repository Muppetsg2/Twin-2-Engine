# SHADE_COMPILER

project(ShaderCompiler)

# Generate a header file with the ORIGIN_FOLDER variable
#configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h COPYONLY)
#file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)
#file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_SOURCE_DIR}/ShadersOrigin SYMBOLIC)

message("ShaderCompiler origin directory: ${SHADER_ORIGIN_FOLDER}")
message("ShaderCompiler build destination directory: ${CMAKE_BINARY_DIR}")

add_executable(SHADER_COMPILER ShaderCompiler.cpp)
#add_compile_definitions(ORIGIN_FOLDER ${SHADER_ORIGIN_FOLDER})

target_include_directories(SHADER_COMPILER PUBLIC ${glad_SOURCE_DIR}
)

target_link_libraries(SHADER_COMPILER PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(SHADER_COMPILER PRIVATE glad
											  glfw
)

