cmake_minimum_required(VERSION 3.1)
project(PhyAnim VERSION 0.0.1
               DESCRIPTION "Physic Based Simulation Library"
               LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
option(LIBIGL_WITH_TETGEN "Use Tetgen" ON)
find_package(LIBIGL REQUIRED SYSTEM)
find_package(OpenMP)
find_package(GLFW3)

if(OpenMP_CXX_FOUND)
  add_definitions(-DPHYANIM_USES_OPENMP)
endif()


add_subdirectory(phyanim)
add_subdirectory(examples)
