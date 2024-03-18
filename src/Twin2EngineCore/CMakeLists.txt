set(TWIN2ENGINE_CORE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(TWIN2ENGINE_CORE_SOURCE_DIR ${TWIN2ENGINE_CORE_DIR})
set(TWIN2ENGINE_CORE_INCLUDE_DIR ${TWIN2ENGINE_CORE_DIR})

file(GLOB_RECURSE TWIN2ENGINE_CORE_SOURCE_FILES 
                        #${GRAPHIC_ENGINE_DIR}/src/*.cpp
                        #${GRAPHIC_ENGINE_DIR}/src/*.c)
                        ${TWIN2ENGINE_CORE_SOURCE_DIR}/*.cpp
                        ${TWIN2ENGINE_CORE_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE TWIN2ENGINE_CORE_PUBLIC_INCLUDE_FILES 
                        #${TWIN2ENGINE_CORE_SOURCE_DIR}/include}/*.h)
                        ${TWIN2ENGINE_CORE_INCLUDE_DIR}/*.h)
						
#file(GLOB_RECURSE TWIN2ENGINE_CORE_PRIVATE_INCLUDE_FILES  
#                        #${TWIN2ENGINE_CORE_SOURCE_DIR}/src/*.h)
#                        ${TWIN2ENGINE_CORE_SOURCE_DIR}/*.h)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${TWIN2ENGINE_CORE_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${TWIN2ENGINE_CORE_PUBLIC_INCLUDE_FILES})
#source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${TWIN2ENGINE_CORE_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(Twin2EngineCore STATIC ${TWIN2ENGINE_CORE_SOURCE_FILES})
#add_library(Twin2EngineCore $<TARGET_OBJECTS:>)

# Adding include to library
target_include_directories(Twin2EngineCore
						PUBLIC 
						${TWIN2ENGINE_CORE_INCLUDE_DIR}
						${glad_SOURCE_DIR}
						
						#PRIVATE
						#${TWIN2ENGINE_CORE_SOURCE_DIR}

)

target_link_libraries(Twin2EngineCore PRIVATE 
										glad
										glm::glm
)
