function(windeployqt target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target ${target} does not exist")
    endif()
    
    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt
        PATHS "${Qt6_DIR}/../../../bin"
        DOC "Path to windeployqt executable"
    )
    
    if(NOT WINDEPLOYQT_EXECUTABLE)
        message(WARNING "windeployqt not found, skipping deployment")
        return()
    endif()
    
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --verbose 1
            --no-compiler-runtime
            --no-angle
            --no-opengl-sw
            "$<TARGET_FILE:${target}>"
        COMMENT "Running windeployqt..."
    )
endfunction()