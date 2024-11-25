/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

cmake_minimum_required(VERSION 3.1)
project(PhyAnim VERSION 0.0.1
               DESCRIPTION "Physic Based Simulation Library"
               LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

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
