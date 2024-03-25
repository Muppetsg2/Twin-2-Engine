# Add source files
#file(GLOB_RECURSE SOURCE_FILES 
#	 *.c
#	 *.cpp)
#	
# Add header files
#file(GLOB_RECURSE HEADER_FILES 
#	 *.h
#	 *.hpp)
# Search for the assets files
#file(GLOB_RECURSE ASSETS_FILES 
#	 ${CMAKE_SOURCE_DIR}/res/*.*)

#set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
#
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})						   
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
#source_group(TREE ${CMAKE_SOURCE_DIR}         FILES ${ASSETS_FILES})
	 
# add_compile_definitions(WITH_MINIAUDIO)
set(GRAPHIC_ENGINE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(GRAPHIC_ENGINE_SOURCE_DIR ${GRAPHIC_ENGINE_DIR}/src)
set(GRAPHIC_ENGINE_INCLUDE_DIR ${GRAPHIC_ENGINE_DIR}/include)

file(GLOB_RECURSE GRAPHIC_ENGINE_SOURCE_FILES 
                        #${GRAPHIC_ENGINE_DIR}/src/*.cpp
                        #${GRAPHIC_ENGINE_DIR}/src/*.c)
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.cpp
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.c)
		

file(GLOB_RECURSE GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES 
                        #${GRAPHIC_ENGINE_DIR}/include}/*.h)
                        ${GRAPHIC_ENGINE_INCLUDE_DIR}/*.h
                        ${GRAPHIC_ENGINE_INCLUDE_DIR}/*.hpp)
						
file(GLOB_RECURSE GRAPHIC_ENGINE_PRIVATE_INCLUDE_FILES  
                        #${GRAPHIC_ENGINE_DIR}/src/*.h)
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.h
                        ${GRAPHIC_ENGINE_SOURCE_DIR}/*.hpp)
	 
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GRAPHIC_ENGINE_SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${GRAPHIC_ENGINE_PRIVATE_INCLUDE_FILES})

# Defining library
add_library(GraphicEngine STATIC ${GRAPHIC_ENGINE_SOURCE_FILES})

# Adding include to library
target_include_directories(GraphicEngine
						PUBLIC 
						#${GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES}
						${GRAPHIC_ENGINE_INCLUDE_DIR}
						${glad_SOURCE_DIR}
						${stb_image_SOURCE_DIR}
						#${GRAPHIC_ENGINE_DIR}
						
						#PRIVATE
						#${GRAPHIC_ENGINE_SOURCE_DIR}

)

#target_link_libraries(GraphicEngine PRIVATE 
#										#glad
#										##stb_image
#										#assimp
#										#glfw
#										#glm::glm
#										#yaml-cpp
#										#spdlog
#										Twin2Engine
#)
target_link_libraries(GraphicEngine ${OPENGL_LIBRARIES})
target_link_libraries(GraphicEngine glad)
target_link_libraries(GraphicEngine stb_image)
target_link_libraries(GraphicEngine assimp)
target_link_libraries(GraphicEngine glfw)
target_link_libraries(GraphicEngine spdlog)
target_link_libraries(GraphicEngine yaml-cpp)
target_link_libraries(GraphicEngine Twin2Engine)
target_link_libraries(GraphicEngine freetype)
target_link_libraries(GraphicEngine glm::glm)

#message("GraphicEngine Public Include Files:")
#foreach(FILE_PATH ${GRAPHIC_ENGINE_PUBLIC_INCLUDE_FILES})
#    message("  ${FILE_PATH}")
#endforeach()
#message("GraphicEngine Private Include Files:")
#foreach(FILE_PATH ${GRAPHIC_ENGINE_PRIVATE_INCLUDE_FILES})
#    message("  ${FILE_PATH}")
#endforeach()
#message("GraphicEngine Source Files:")
#foreach(FILE_PATH ${GRAPHIC_ENGINE_SOURCE_FILES})
#    message("  ${FILE_PATH}")
#endforeach()

