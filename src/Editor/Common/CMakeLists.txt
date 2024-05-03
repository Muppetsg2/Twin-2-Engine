project(Common VERSION 1.0)


set(Common_DIR ${CMAKE_CURRENT_LIST_DIR})
set(Common_SOURCE_DIR ${Common_DIR}/src)
set(Common_INCLUDE_DIR ${Common_DIR}/include)
set(Common_PRECOMPILE_HEADER ${Common_DIR}/pch.h)

file(GLOB_RECURSE Common_SOURCE_FILES 
                        ${Common_SOURCE_DIR}/*.cpp
                        ${Common_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE Common_PUBLIC_INCLUDE_FILES 
                        ${Common_INCLUDE_DIR}/*.h
                        ${Common_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE Common_PRIVATE_INCLUDE_FILES  
                        ${Common_SOURCE_DIR}/*.h
                        ${Common_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${Common_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${Common_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${Common_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${Common_SOURCE_FILES})

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${Common_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${Common_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PRIVATE Twin2Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)