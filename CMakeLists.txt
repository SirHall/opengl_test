cmake_minimum_required(VERSION 3.5)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wno-unused-parameter -Iinclude -g)

project(out)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_NAME "out")
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Copy resource files to build output 
file(COPY res DESTINATION .)

file(GLOB_RECURSE SRCS
    "src/*.cpp"
    RELATIVE_PATH
)


# OpenGL values
set(OpenGL_GL_PREFERENCE GLVND)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Setup Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
message(STATUS "SDL2_Libs: ${CONAN_LIBS}")

add_executable(out ${SRCS})
target_include_directories(out PUBLIC ${INCLUDE_DIR})
target_link_libraries(out ${CONAN_LIBS})


# add_subdirectory(dep/glfw)
# target_link_libraries(out glfw)
# find_package(OpenGL REQUIRED)

# add_subdirectory(dep/glew/build/cmake)
# target_link_libraries(out glew)

# target_include_directories(out PUBLIC ${OPENGL_INCLUDE_DIR})
# target_link_libraries(out ${OPENGL_gl_LIBRARY}) 

# Setup and include libraries
# find_package(Boost COMPONENTS system filesystem REQUIRED)
# include_directories(${Boost_INCLUDE_DIR})
# target_link_libraries(out ${Boost_LIBRARIES})