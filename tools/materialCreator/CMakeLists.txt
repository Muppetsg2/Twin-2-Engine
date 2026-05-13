# MATERIAL_CREATOR

project(MaterialCreator)

file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_SOURCE_DIR}/ShadersOrigin SYMBOLIC)
file(CREATE_LINK ${SHADER_ORIGIN_FOLDER}/res ${CMAKE_CURRENT_BINARY_DIR}/ShadersOrigin SYMBOLIC)

add_executable(${PROJECT_NAME} MaterialCreator.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)

#set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

file(CREATE_LINK ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bin SYMBOLIC)

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