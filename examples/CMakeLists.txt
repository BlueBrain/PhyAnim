/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/PhyAnim>
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

# Data copy to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION
  ${CMAKE_BINARY_DIR}/data)

include_directories(${CMAKE_BINARY_DIR}/include)

file(GLOB  PYTHON_FILES "*.py")
file(COPY ${PYTHON_FILES} DESTINATION
  ${CMAKE_BINARY_DIR})

add_compile_definitions(DATAPATH="${CMAKE_BINARY_DIR}/data")

add_subdirectory(appCheckCollisions)
add_subdirectory(appFormatConverter)
add_subdirectory(appTetrahedralizeMesh)
add_subdirectory(appOverlapCircuit)
add_subdirectory(appOverlapCollisions)
add_subdirectory(appCollidingSomas)

if(GLFW3_FOUND)
  add_subdirectory(appRenderMesh)
  add_subdirectory(appAnimMesh)
  add_subdirectory(appMoveMesh)
  add_subdirectory(appRenderMorpho)
  add_subdirectory(appOverlapCircuitViz)
  add_subdirectory(appOverlapCollisionsViz)
  add_subdirectory(appCollidingSomasViz)
  add_subdirectory(appSceneGenerator)
  add_subdirectory(appSomaGenerator)
  add_subdirectory(appDemo)
endif()
