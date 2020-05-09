cmake_minimum_required(VERSION 3.14)
project(Shooter)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

# настраиваем glfw
add_subdirectory(external/glfw-3.3.2)
include_directories(external/glfw-3.3.2/include)

# настраиваем glew

add_definitions(
        -DGLEW_STATIC
)

set(GLEW_SOURCE
        external/glew-2.1.0/src/glew.c
        external/glew-2.1.0/src/glewinfo.c
        external/glew-2.1.0/src/visualinfo.c)
set(GLEW_HEADERS
        external/glew-2.1.0/include/GL/eglew.h
        external/glew-2.1.0/include/GL/glew.h
        external/glew-2.1.0/include/GL/glxew.h
        external/glew-2.1.0/include/GL/wglew.h)

include_directories(external/glew-2.1.0/include)

add_library(glew STATIC ${GLEW_SOURCE} ${GLEW_HEADERS})


#настраиваем glm

include_directories(
        external/glm-0.9.7.1/)

add_definitions(
        -DGLM_SWIZZLE
)

#настраиваем stb
add_definitions(
        -DSTB_IMAGE_IMPLEMENTATION
)

# собираем всё вместе
add_executable(Shooter
        main.cpp
        shader.h shader.cpp
        stb_image.h
        icosahedron.h icosahedron.cpp
        sphere.h sphere.cpp
        utilities.h utilities.cpp
        camera.h camera.cpp)

target_link_libraries(Shooter
        glfw
        glew
        ${GLFW_LIBRARIES}
        ${OPENGL_LIBRARY}
        )