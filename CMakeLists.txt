cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(Twin2Game VERSION 1.0)

set_property(GLOBAL PROPERTY ISMINIAUDIO false)
set_property(GLOBAL PROPERTY USEIMGUI true)

IF (WIN32)
  message(NOTICE "\n")
  message(NOTICE "********************** IMPORTANT NOTICE ****************************\n")
  message(NOTICE "Make sure you have enabled Developer Mode in your system settings!")
  message(NOTICE "To enable Developer Mode go to Settings -> Update & Security -> For developers -> Developer Mode ON")
  message(NOTICE "\n********************************************************************")
  message(NOTICE "\n")
ENDIF()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(global_settings)

# 
#set(GLSLANG_VALIDATOR_EXECUTABLE "/tools/glslang/bin/glslangValidator.exe")
#execute_process(
#
#    COMMAND ${GLSLANG_VALIDATOR_EXECUTABLE} -V ${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader.vert -o ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.spv
#    RESULT_VARIABLE result
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)
## Check the result variable to determine if the command succeeded or failed
#if(result EQUAL 0)
#    message(STATUS "glslangValidator succeeded")
#else()
#    message(FATAL_ERROR "glslangValidator failed with error code ${result}")
#endif()

set(SHADER_ORIGIN_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})


# ---- Dependencies ----
add_subdirectory(thirdparty)

# ---- Tools ----
add_subdirectory(tools)

# ---- Main project's files ----
add_subdirectory(src)
