# CPM - package manager
include(CPM)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
set(stb_image_SOURCE_DIR ${STB_IMAGE_DIR} CACHE INTERNAL "")
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp)
target_include_directories(stb_image PRIVATE ${STB_IMAGE_DIR})

# free type
#set(FREETYPE_DIR ${CMAKE_CURRENT_LIST_DIR}/freetype)
#set(freetype_SOURCE_DIR ${FREETYPE_DIR}/include CACHE INTERNAL "")
#add_library(freetype STATIC ${FREETYPE_DIR}/src/freetype.cpp)
#target_include_directories(freetype PRIVATE ${FREETYPE_DIR}/include)

# tinygltf
#set(TINY_GTLF_DIR ${CMAKE_CURRENT_LIST_DIR}/tiny_gltf)
#set(tinygltf_SOURCE_DIR ${TINY_GTLF_DIR} CACHE INTERNAL "")
#add_library(tinygltf STATIC ${TINY_GTLF_DIR}/tiny_gltf.cc)
##target_link_libraries(tinygltf PRIVATE stb_image)
##target_include_directories(tinygltf PRIVATE ${TINY_GTLF_DIR} ${stb_image_SOURCE_DIR})
#target_include_directories(tinygltf PRIVATE ${TINY_GTLF_DIR})

if(IS_MINI_AUDIO)
    # miniaudio
    set(MINIAUDIO_DIR ${CMAKE_CURRENT_LIST_DIR}/miniaudio)
    set(miniaudio_SOURCE_DIR ${MINIAUDIO_DIR}/include CACHE INTERNAL "")
    file(GLOB_RECURSE miniaudio_SOURCE_FILES 
                            ${MINIAUDIO_DIR}/src/*.cpp
                            ${MINIAUDIO_DIR}/src/*.c)
    add_library(miniaudio STATIC ${miniaudio_SOURCE_FILES})
    target_include_directories(miniaudio PRIVATE ${MINIAUDIO_DIR}/include)

    set_target_properties(miniaudio PROPERTIES FOLDER "thirdparty")
else()
    # soloud
    add_compile_definitions(WITH_MINIAUDIO)
    set(SOLOUD_DIR ${CMAKE_CURRENT_LIST_DIR}/soloud)
    set(soloud_SOURCE_DIR ${SOLOUD_DIR}/include CACHE INTERNAL "")
    file(GLOB_RECURSE soloud_SOURCE_FILES 
                            ${SOLOUD_DIR}/src/*.cpp
                            ${SOLOUD_DIR}/src/*.c)
    add_library(soloud STATIC ${soloud_SOURCE_FILES})
    target_include_directories(soloud PRIVATE ${SOLOUD_DIR}/include)
    set_target_properties(soloud PROPERTIES FOLDER "thirdparty")
endif()

# other
CPMAddPackage("gh:assimp/assimp@5.4.3")
CPMAddPackage("gh:glfw/glfw#3.4")
#CPMAddPackage("gh:syoyo/tinygltf@2.8.21")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:gabime/spdlog@1.14.1")
CPMAddPackage("gh:jbeder/yaml-cpp#0.8.0")
CPMAddPackage("gh:freetype/freetype#VER-2-13-3")

set_target_properties(glad
                      stb_image 
                      assimp 
                      glfw 
                      glm 
                      spdlog
                      yaml-cpp
                      #tinygltf
                      freetype PROPERTIES FOLDER "thirdparty")

if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")

    CPMAddPackage("gh:ocornut/imgui@1.91.3-docking")

    set(imgui_SOURCE_DIR ${imgui_SOURCE_DIR} CACHE INTERNAL "")
    add_library(imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp
					         ${imgui_SOURCE_DIR}/imgui_demo.cpp
					         ${imgui_SOURCE_DIR}/imgui_draw.cpp
						     ${imgui_SOURCE_DIR}/imgui_tables.cpp
					         ${imgui_SOURCE_DIR}/imgui_widgets.cpp)

    set_target_properties(imgui PROPERTIES FOLDER "thirdparty")

    if(FIND_MEMORY_LEAKS)
        set(TRACEY_LEAK_DIR ${CMAKE_CURRENT_LIST_DIR}/tracey_leak)
        set(tracey_leak_SOURCE_DIR ${TRACEY_LEAK_DIR}/include CACHE INTERNAL "")
        file(GLOB_RECURSE tracey_leak_SOURCE_FILES 
                                ${TRACEY_LEAK_DIR}/src/*.cpp
                                ${TRACEY_LEAK_DIR}/src/*.c)
        add_library(tracey_leak STATIC ${tracey_leak_SOURCE_FILES})
        target_include_directories(tracey_leak PRIVATE ${TRACEY_LEAK_DIR}/include)

        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads REQUIRED)
        set_target_properties(tracey_leak PROPERTIES FOLDER "thirdparty")
    endif()
endif()

if(TRACY_PROFILER)
    CPMAddPackage("gh:wolfpld/tracy@0.11.1")
    set_target_properties(TracyClient PROPERTIES FOLDER "thirdparty")
endif()

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()