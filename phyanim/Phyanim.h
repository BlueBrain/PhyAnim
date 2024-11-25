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