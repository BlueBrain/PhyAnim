/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
