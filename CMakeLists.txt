cmake_minimum_required(VERSION 3.1)
project(PhyAnim VERSION 0.0.1
               DESCRIPTION "Physic Based Simulation Library"
               LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)


find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenMP)
find_package(GLUT)
find_package(GLFW3)
option(LIBIGL_WITH_TETGEN "Use Tetgen" ON)
find_package(LIBIGL QUIET)

if(OpenMP_CXX_FOUND)
  add_definitions(-DPHYANIM_USES_OPENMP)
endif()


add_subdirectory(phyanim)
add_subdirectory(examples)
