project(GameScripts VERSION 1.0)

set(GAMESCRIPTS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(GAMESCRIPTS_SOURCE_DIR ${GAMESCRIPTS_DIR}/src)
set(GAMESCRIPTS_INCLUDE_DIR ${GAMESCRIPTS_DIR}/include)
set(GAMESCRIPTS_PRECOMPILE_HEADER ${GAMESCRIPTS_DIR}/pch.h)

file(GLOB_RECURSE GAMESCRIPTS_SOURCE_FILES 
                        ${GAMESCRIPTS_SOURCE_DIR}/*.cpp
                        ${GAMESCRIPTS_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE GAMESCRIPTS_PUBLIC_INCLUDE_FILES 
                        ${GAMESCRIPTS_INCLUDE_DIR}/*.h
                        ${GAMESCRIPTS_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE GAMESCRIPTS_PRIVATE_INCLUDE_FILES  
                        ${GAMESCRIPTS_SOURCE_DIR}/*.h
                        ${GAMESCRIPTS_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GAMESCRIPTS_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GAMESCRIPTS_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GAMESCRIPTS_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${GAMESCRIPTS_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif()

# Adding include to library
target_include_directories(${PROJECT_NAME} PUBLIC ${GAMESCRIPTS_INCLUDE_DIR})

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${GAMESCRIPTS_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PRIVATE Twin2Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE Tilemap)
target_link_libraries(${PROJECT_NAME} PRIVATE Tools)
target_link_libraries(${PROJECT_NAME} PRIVATE EngineAlgorithms)
target_link_libraries(${PROJECT_NAME} PRIVATE GraphicEngine)
target_link_libraries(${PROJECT_NAME} PRIVATE Generation)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE StateMachine)
target_link_libraries(${PROJECT_NAME} PRIVATE DecisionTree)

if (TRACY_PROFILER)
    target_link_libraries(${PROJECT_NAME} PUBLIC TracyClient)
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	 target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
	 target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
endif()

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