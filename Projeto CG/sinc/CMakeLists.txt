# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (sinc)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
  message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
  message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
  message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



set(ALL_LIBS
  GLEW::glew
  ${OPENGL_LIBRARY}
  glfw 
  glm::glm
  assimp::assimp
)

add_definitions(
 -DTW_STATIC
 -DTW_NO_LIB_PRAGMA
 -DTW_NO_DIRECT3D
 -DGLEW_STATIC
 -D_CRT_SECURE_NO_WARNINGS
)

add_executable(sinc
 main.cpp
 textFile.cpp
 ../Sphere.h
 ../textFile.h
 ../stb_image.h
 ../ftbuild2.h
 ../common/shader.hpp
 ../common/texture.hpp
 ../common/controls.hpp
 ../common/shader.cpp
 ../common/texture.cpp
 ../common/controls.cpp


 test.frag
 test.vert
)

target_link_libraries(sinc ${ALL_LIBS})
