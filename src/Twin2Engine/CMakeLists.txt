project(Twin2Engine VERSION 1.0)

set(Twin2Engine_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(Twin2Engine_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(Twin2Engine_PRECOMPILE_HEADERS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	 ${Twin2Engine_SOURCE_DIR}/*.c
	 ${Twin2Engine_SOURCE_DIR}/*.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES
	 ${Twin2Engine_INCLUDE_DIR}/*.h
	 ${Twin2Engine_INCLUDE_DIR}/*.hpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	list(FILTER SOURCE_FILES EXCLUDE REGEX ".*(imgui_impl\/).*")
	list(FILTER HEADER_FILES EXCLUDE REGEX ".*(imgui_impl\/).*")
endif()
	 
# Search for the assets files
file(GLOB_RECURSE ASSETS_FILES 
	 ${CMAKE_SOURCE_DIR}/res/*.*)

set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}         FILES ${ASSETS_FILES})

add_subdirectory(GraphicEngine)
add_subdirectory(PhysicEngine)
add_subdirectory(Tools)

# ---- Dependency ----
add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")
target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG="$<IF:$<CONFIG:Debug>,1,0>")

if (TRACY_PROFILER)
    target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
	target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_PROFILER)
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC ${Twin2Engine_PRECOMPILE_HEADERS_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC	${Twin2Engine_INCLUDE_DIR}
													${GRAPHIC_ENGINE_INCLUDE_DIR}
													${PHYSIC_ENGINE_INCLUDE_DIR}
													${TOOLS_INCLUDE_DIR})

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	 target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
	 target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
endif()

if(IS_MINI_AUDIO)
	target_link_libraries(${PROJECT_NAME} PUBLIC miniaudio)
	target_include_directories(${PROJECT_NAME} PUBLIC ${miniaudio_SOURCE_DIR})
else()
	target_link_libraries(${PROJECT_NAME} PUBLIC soloud)
	target_include_directories(${PROJECT_NAME} PUBLIC ${soloud_SOURCE_DIR})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC glfw
										GraphicEngine
										PhysicEngine
										#Tilemap
										#Generation
										GameScripts
										Tools)
										
target_compile_options(${PROJECT_NAME} PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:
		$<$<CONFIG:Release>:/O2>
		$<$<CONFIG:Release>:/GL>
		$<$<CONFIG:Release>:/Zc:preprocessor>
		$<$<CONFIG:Release>:/fp:fast>
		$<$<CONFIG:Debug>:/RTC1>
	>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:GNU>>>:
		$<$<CONFIG:Release>:-O3>
		$<$<CONFIG:Release>:-flto=auto>
	>
)

target_link_options(${PROJECT_NAME} PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:
		$<$<CONFIG:Release>:/LTCG>
		$<$<CONFIG:Release>:/O2>
	>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:GNU>>>:
		$<$<CONFIG:Release>:-flto=auto>
		$<$<CONFIG:Release>:-O3>
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:Clang>>:-fuse-ld=lld>
		-static
	>
)