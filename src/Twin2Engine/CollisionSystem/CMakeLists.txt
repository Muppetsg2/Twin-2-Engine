project(CollisionSystem VERSION 1.0)

set(COLLISION_SYSTEM_DIR ${CMAKE_CURRENT_LIST_DIR})
set(COLLISION_SYSTEM_SOURCE_DIR ${COLLISION_SYSTEM_DIR}/src)
set(COLLISION_SYSTEM_INCLUDE_DIR ${COLLISION_SYSTEM_DIR}/include)
set(COLLISION_SYSTEM_PRECOMPILE_HEADER ${COLLISION_SYSTEM_DIR}/pch.h)

file(GLOB_RECURSE COLLISION_SYSTEM_SOURCE_FILES 
                        ${COLLISION_SYSTEM_SOURCE_DIR}/*.cpp
                        ${COLLISION_SYSTEM_SOURCE_DIR}/*.c)


file(GLOB_RECURSE COLLISION_SYSTEM_PUBLIC_INCLUDE_FILES 
                        ${COLLISION_SYSTEM_INCLUDE_DIR}/*.h
                        ${COLLISION_SYSTEM_INCLUDE_DIR}/*.hpp)

file(GLOB_RECURSE COLLISION_SYSTEM_PRIVATE_INCLUDE_FILES
                        ${COLLISION_SYSTEM_SOURCE_DIR}/*.h
                        ${COLLISION_SYSTEM_SOURCE_DIR}/*.hpp)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${COLLISION_SYSTEM_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${COLLISION_SYSTEM_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${COLLISION_SYSTEM_PRIVATE_INCLUDE_FILES})


#Defining library
add_library(${PROJECT_NAME} STATIC ${COLLISION_SYSTEM_SOURCE_FILES})

#Adding precompile header
target_precompile_headers(${PROJECT_NAME} PUBLIC ${COLLISION_SYSTEM_PRECOMPILE_HEADER})

#Adding include to library
target_include_directories(${PROJECT_NAME}
                        PUBLIC 
                        ${COLLISION_SYSTEM_INCLUDE_DIR}
                        ${glad_SOURCE_DIR}

                        PRIVATE
                        ${COLLISION_SYSTEM_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog
                                              glm::glm)