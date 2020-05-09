####################
## GLFW and OpenGL##
####################

include_directories(
        glfw-3.3.2/include/GLFW/
        glew-2.1.0/include/
)

# собираем glfw
add_subdirectory (glfw-3.3.2)

### GLEW ###

find_package(GLEW 2.1.0 EXACT REQUIRED PATHS glew-2.1.0)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()