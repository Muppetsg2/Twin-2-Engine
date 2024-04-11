project(Generation VERSION 0.0)

set(GENERATION_DIR ${CMAKE_CURRENT_LIST_DIR})
set(GENERATION_SOURCE_DIR ${GENERATION_DIR}/src)
set(GENERATION_INCLUDE_DIR ${GENERATION_DIR}/include)
set(GENERATION_PRECOMPILE_HEADER ${GENERATION_DIR}/pch.h)

file(GLOB_RECURSE GENERATION_SOURCE_FILES 
                        ${GENERATION_SOURCE_DIR}/*.cpp
                        ${GENERATION_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE GENERATION_PUBLIC_INCLUDE_FILES 
                        ${GENERATION_INCLUDE_DIR}/*.h
                        ${GENERATION_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE GENERATION_PRIVATE_INCLUDE_FILES  
                        ${GENERATION_SOURCE_DIR}/*.h
                        ${GENERATION_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GENERATION_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GENERATION_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GENERATION_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${GENERATION_SOURCE_FILES})

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${GENERATION_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${GENERATION_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PRIVATE Twin2Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE Tilemap)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
#target_link_libraries(GraphicEngine yaml-cpp)