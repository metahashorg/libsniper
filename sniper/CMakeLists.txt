if (DEFINED libsniper)
    foreach(dir ${libsniper})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
            add_subdirectory(${dir})
        endif()
    endforeach()
endif()
