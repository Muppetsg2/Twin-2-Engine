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

get_property(is_miniaudio GLOBAL PROPERTY ISMINIAUDIO)

if(is_miniaudio)
    # miniaudio
    set(MINIAUDIO_DIR ${CMAKE_CURRENT_LIST_DIR}/miniaudio)
    set(miniaudio_SOURCE_DIR ${MINIAUDIO_DIR}/include CACHE INTERNAL "")
    file(GLOB_RECURSE miniaudio_SOURCE_FILES 
                            ${MINIAUDIO_DIR}/src/*.cpp
                            ${MINIAUDIO_DIR}/src/*.c)
    add_library(miniaudio STATIC ${miniaudio_SOURCE_FILES})
    target_include_directories(miniaudio PRIVATE ${MINIAUDIO_DIR}/include)
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
endif()

# other
CPMAddPackage("gh:assimp/assimp@5.3.1")
CPMAddPackage("gh:glfw/glfw#3.4")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:gabime/spdlog@1.13.0")
CPMAddPackage("gh:jbeder/yaml-cpp#0.8.0")
CPMAddPackage("gh:freetype/freetype#VER-2-13-2")

get_property(is_imgui GLOBAL PROPERTY USEIMGUI)

if(is_imgui)
    CPMAddPackage("gh:ocornut/imgui@1.90.4")

    set(imgui_SOURCE_DIR ${imgui_SOURCE_DIR} CACHE INTERNAL "")
    add_library(imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp
					         ${imgui_SOURCE_DIR}/imgui_demo.cpp
					         ${imgui_SOURCE_DIR}/imgui_draw.cpp
						     ${imgui_SOURCE_DIR}/imgui_tables.cpp
					         ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
endif()
                         
if(is_miniaudio)
    if (is_imgui)
        set_target_properties(glad
                              stb_image 
                              assimp 
                              glfw 
                              glm 
                              imgui
                              spdlog
                              yaml-cpp
                              miniaudio
                              freetype PROPERTIES FOLDER "thirdparty")
    else()
        set_target_properties(glad
                        stb_image 
                        assimp 
                        glfw 
                        glm 
                        spdlog
                        yaml-cpp
                        miniaudio
                        freetype PROPERTIES FOLDER "thirdparty")
    endif()
else()
    if (is_imgui)
        set_target_properties(glad
                              stb_image 
                              assimp 
                              glfw 
                              glm 
                              imgui
                              spdlog 
                              soloud
                              yaml-cpp
                              freetype PROPERTIES FOLDER "thirdparty")
    else()
        set_target_properties(glad
                        stb_image 
                        assimp 
                        glfw 
                        glm 
                        spdlog 
                        soloud
                        yaml-cpp
                        freetype PROPERTIES FOLDER "thirdparty")
    endif()
endif()

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()