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

file(REMOVE ${CMAKE_BINARY_DIR}/include)

file(GLOB_RECURSE PHYANIM_SOURCE_FILES "*.cpp")
file(GLOB_RECURSE PHYANIM_HEADER_FILES "*.h")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR}/include FILES_MATCHING 
  PATTERN "*.h")



add_library(phyanim ${PHYANIM_SOURCE_FILES} ${PHYANIM_HEADER_FILES})

target_include_directories(phyanim PUBLIC ${EIGEN3_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})

target_link_libraries(phyanim PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} 
  igl::core)

if(OpenMP_CXX_FOUND)
  target_link_libraries(phyanim PUBLIC OpenMP::OpenMP_CXX)
endif()

if (MorphIO_FOUND AND sonata_FOUND)
  target_link_libraries(phyanim PUBLIC MorphIO::morphio sonata::sonata_shared)
endif()