cmake_minimum_required(VERSION 3.1)
project(PhyAnim VERSION 0.0.1
               DESCRIPTION "Physic Based Simulation Library"
               LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
option(LIBIGL_WITH_TETGEN "Use Tetgen" ON)
find_package(LIBIGL REQUIRED SYSTEM)
find_package(glm REQUIRED)
find_package(OpenMP)
find_package(GLFW3)
find_package(sonata)
find_package(MorphIO)
find_package(Python REQUIRED Development)

if(OpenMP_CXX_FOUND)
  add_definitions(-DPHYANIM_USES_OPENMP)
endif()

if (GLFW3_FOUND)
  add_definitions(-DPHYANIM_USES_GLFW3)
endif()


if(MorphIO_FOUND AND sonata_FOUND)
  add_definitions(-DPHYANIM_USES_MORPHO)
endif()


add_subdirectory(phyanim)
add_subdirectory(examples)
