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

#ifndef __EXAMPLES_MORPHO_H
#define __EXAMPLES_MORPHO_H

#include <phyanim/Phyanim.h>

using namespace phyanim;

namespace examples
{
typedef geometry::Nodes Section;
typedef std::vector<Section*> Sections;

enum RadiusFunc
{
    MIN_NEURITES,
    MAX_NEURITES,
    MEAN_NEURITES,
    MIN_SOMAS,
    MAX_SOMAS,
    MEAN_SOMAS
};

class Morpho
{
public:
    Morpho(std::string path,
           geometry::Mat4 mat = geometry::Mat4(1.0f),
           RadiusFunc radiusFunc = RadiusFunc::MAX_NEURITES,
           bool loadNeurites = true);

    ~Morpho(){};

    void print();

    void cutout(geometry::AxisAlignedBoundingBox& aabb);

    geometry::Nodes nodes;

    geometry::Nodes somaNodes;

    geometry::NodePtr soma;

    geometry::Nodes sectionNodes;

    Sections sections;

    geometry::HierarchicalAABBPtr aabb;

    geometry::Edges edges;

    geometry::Vec3 color;

    geometry::Vec3 collColor;

    geometry::Vec3 fixColor;
};

}  // namespace examples

#endif
