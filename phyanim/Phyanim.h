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

#ifndef __PHYANIM_PHYANIM__
#define __PHYANIM_PHYANIM__

#include <phyanim/anim/AnimSystem.h>
#include <phyanim/anim/CollisionDetection.h>
#include <phyanim/anim/ExplicitMassSpringSystem.h>
#include <phyanim/anim/ImplicitFEMSystem.h>
#include <phyanim/geometry/AxisAlignedBoundingBox.h>
#include <phyanim/geometry/Edge.h>
#include <phyanim/geometry/HierarchicalAABB.h>
#include <phyanim/geometry/Math.h>
#include <phyanim/geometry/Mesh.h>
#include <phyanim/geometry/Node.h>
#include <phyanim/geometry/Primitive.h>
#include <phyanim/geometry/Tetrahedron.h>
#include <phyanim/geometry/Triangle.h>
#include <phyanim/graphics/Camera.h>
#include <phyanim/graphics/ColorPalette.h>
#include <phyanim/graphics/Mesh.h>
#include <phyanim/graphics/RenderProgram.h>
#include <phyanim/graphics/Scene.h>
#include <phyanim/graphics/SkyBox.h>
#include <phyanim/graphics/Texture.h>

#endif