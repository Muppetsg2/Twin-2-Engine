project(EngineAlgorithms VERSION 1.0)


set(EngineAlgorithms_DIR ${CMAKE_CURRENT_LIST_DIR})
set(EngineAlgorithms_SOURCE_DIR ${EngineAlgorithms_DIR}/src)
set(EngineAlgorithms_INCLUDE_DIR ${EngineAlgorithms_DIR}/include)
set(EngineAlgorithms_PRECOMPILE_HEADER ${EngineAlgorithms_DIR}/pch.h)

file(GLOB_RECURSE EngineAlgorithms_SOURCE_FILES 
                        ${EngineAlgorithms_SOURCE_DIR}/*.cpp
                        ${EngineAlgorithms_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE EngineAlgorithms_PUBLIC_INCLUDE_FILES 
                        ${EngineAlgorithms_INCLUDE_DIR}/*.h
                        ${EngineAlgorithms_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE EngineAlgorithms_PRIVATE_INCLUDE_FILES  
                        ${EngineAlgorithms_SOURCE_DIR}/*.h
                        ${EngineAlgorithms_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${EngineAlgorithms_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${EngineAlgorithms_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${EngineAlgorithms_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${EngineAlgorithms_SOURCE_FILES})

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${EngineAlgorithms_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${EngineAlgorithms_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
#target_link_libraries(${PROJECT_NAME} yaml-cpp)