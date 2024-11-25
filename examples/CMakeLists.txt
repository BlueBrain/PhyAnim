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
