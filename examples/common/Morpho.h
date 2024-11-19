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
