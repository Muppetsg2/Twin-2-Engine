file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(subdir ${subdirs})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
        message(${subdir})
        add_subdirectory(${subdir})
        target_link_libraries(${PROJECT_NAME} PUBLIC ${subdir})
    endif()
endforeach()