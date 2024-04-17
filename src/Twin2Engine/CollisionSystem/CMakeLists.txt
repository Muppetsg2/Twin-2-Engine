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
                        #${GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES}
                        ${COLLISION_SYSTEM_INCLUDE_DIR}
                        ${glad_SOURCE_DIR}
                        #${GRAPHIC_ENGINE_DIR}

                        PRIVATE
                        ${COLLISION_SYSTEM_DIR}
                        )

target_link_libraries(${PROJECT_NAME} PRIVATE 
                                        #glad
                                        #glm::glm
                                        Twin2Engine
                                        spdlog
)


## Add source files
#file(GLOB_RECURSE SOURCE_FILES 
#	 src/*.c
#	 src/*.cpp)
#	
## Add header files
#file(GLOB_RECURSE HEADER_FILES
#	 inc/*.h
#	 inc/*.hpp)
#	 
#
##set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
#
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})						   
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
#
## ---- Dependency ----
#set(COLLISIONSYSTEM_LIBRARY CollisionSystem)
#add_library(${COLLISIONSYSTEM_LIBRARY} STATIC ${SOURCE_FILES})
#
#target_include_directories(${COLLISIONSYSTEM_LIBRARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
##target_compile_definitions(${COLLISIONSYSTEM_LIBRARY} PRIVATE GLFW_INCLUDE_NONE)
##target_compile_definitions(${COLLISIONSYSTEM_LIBRARY} PRIVATE LIBRARY_SUFFIX="")
#
#target_link_libraries(${COLLISIONSYSTEM_LIBRARY} ${OPENGL_LIBRARIES})
#target_link_libraries(${COLLISIONSYSTEM_LIBRARY} glad)
##target_link_libraries(${COLLISIONSYSTEM_LIBRARY} glfw)
#target_link_libraries(${COLLISIONSYSTEM_LIBRARY} glm::glm)