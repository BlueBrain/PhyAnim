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