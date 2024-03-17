# SHADE_COMPILER

project(ShaderCompiler)

# Generate a header file with the ORIGIN_FOLDER variable
configure_file(config.h.in config.h)

message("Shader origin directory: ${SHADER_ORIGIN_FOLDER}")

add_executable(SHADER_COMPILER ShaderCompiler.cpp)
#add_compile_definitions(ORIGIN_FOLDER ${SHADER_ORIGIN_FOLDER})

target_include_directories(SHADER_COMPILER PUBLIC ${glad_SOURCE_DIR}
)

target_link_libraries(SHADER_COMPILER PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(SHADER_COMPILER PRIVATE glad
											  glfw
)