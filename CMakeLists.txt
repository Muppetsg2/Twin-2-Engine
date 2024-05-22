cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(Twin2Game VERSION 1.0)

set_property(GLOBAL PROPERTY IS_MINI_AUDIO false)
set_property(GLOBAL PROPERTY FIND_MEMORY_LEAKS false)
set_property(GLOBAL PROPERTY TRACY_PROFILER true)

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

#creating global variables

#shader variables
#set(SHADER_ORIGIN_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})

# ---- Dependencies ----
add_subdirectory(thirdparty)

# ---- Tools ----
#add_subdirectory(tools)

# ---- Main project's files ----
add_subdirectory(src)