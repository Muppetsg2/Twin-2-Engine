project(Humans VERSION 1.0)

set(Humans_PRIORITY 0 CACHE STRING "Priority")
set(PROJECT_PRIORITY 0 CACHE STRING "Priority")


set(HUMANS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(HUMANS_SOURCE_DIR ${HUMANS_DIR}/src)
set(HUMANS_INCLUDE_DIR ${HUMANS_DIR}/include)
set(HUMANS_PRECOMPILE_HEADER ${HUMANS_DIR}/pch.h)

file(GLOB_RECURSE HUMANS_SOURCE_FILES 
                        ${HUMANS_SOURCE_DIR}/*.cpp
                        ${HUMANS_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE HUMANS_PUBLIC_INCLUDE_FILES 
                        ${HUMANS_INCLUDE_DIR}/*.h
                        ${HUMANS_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE HUMANS_PRIVATE_INCLUDE_FILES  
                        ${HUMANS_SOURCE_DIR}/*.h
                        ${HUMANS_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${HUMANS_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${HUMANS_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${HUMANS_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(${PROJECT_NAME} STATIC ${HUMANS_SOURCE_FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

# Adding include to library
target_include_directories(${PROJECT_NAME}
						PUBLIC 
                        ${HUMANS_INCLUDE_DIR}
)

# Adding precompile Headers to library
target_precompile_headers(${PROJECT_NAME} PUBLIC ${HUMANS_PRECOMPILE_HEADER})


target_link_libraries(${PROJECT_NAME} PRIVATE Twin2Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE PhysicEngine)
target_link_libraries(${PROJECT_NAME} PRIVATE Generation)
target_link_libraries(${PROJECT_NAME} PRIVATE Tilemap)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)