#project(CollisionSystem VERSION 1.0)

set(LIGHTING_SYSTEM_DIR ${CMAKE_CURRENT_LIST_DIR})
set(LIGHTING_SYSTEM_SOURCE_DIR ${LIGHTING_SYSTEM_DIR}/src)
set(LIGHTING_SYSTEM_INCLUDE_DIR ${LIGHTING_SYSTEM_DIR}/include)

file(GLOB_RECURSE LIGHTING_SYSTEM_SOURCE_FILES 
                        #${GRAPHIC_ENGINE_DIR}/src/.cpp
                        #${GRAPHIC_ENGINE_DIR}/src/.c)
                        ${LIGHTING_SYSTEM_SOURCE_DIR}/*.cpp
                        ${LIGHTING_SYSTEM_SOURCE_DIR}/*.c)


file(GLOB_RECURSE LIGHTING_SYSTEM_PUBLIC_INCLUDE_FILES 
                        ${LIGHTING_SYSTEM_INCLUDE_DIR}/*.h)

file(GLOB_RECURSE LIGHTING_SYSTEM_PRIVATE_INCLUDE_FILES
                        ${LIGHTING_SYSTEM_SOURCE_DIR}/*.h)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${LIGHTING_SYSTEM_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${LIGHTING_SYSTEM_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${LIGHTING_SYSTEM_PRIVATE_INCLUDE_FILES})


#Defining library
add_library(LightingSystem STATIC ${LIGHTING_SYSTEM_SOURCE_FILES})

#Adding include to library
target_include_directories(LightingSystem
                        PUBLIC 
                        #${GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES}
                        ${LIGHTING_SYSTEM_INCLUDE_DIR}
                        ${glad_SOURCE_DIR}
                        #${GRAPHIC_ENGINE_DIR}

                        PRIVATE
                        ${LIGHTING_SYSTEM_DIR}
                        )

target_link_libraries(LightingSystem PRIVATE 
                                        glad
                                        glfw
                                        #glm::glm
                                        Twin2Engine
                                        GraphicEngine
                                        spdlog
                                        
)