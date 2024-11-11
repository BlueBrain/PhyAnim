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

#include "Morpho.h"

#ifdef PHYANIM_USES_MORPHO
#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

#endif

#include <stack>

using namespace phyanim;

namespace examples
{
Morpho::Morpho(std::string path,
               geometry::Mat4 mat,
               RadiusFunc radiusFunc,
               bool loadNeurites)
    : color(0.2, 0.8, 0.2)
    , collColor(1.0, 0.0, 0.0)
    , fixColor(0.2, 0.2, 0.2)
{
#ifdef PHYANIM_USES_MORPHO
    auto morpho = morphio::Morphology(path);

    if (loadNeurites)
    {
        for (auto section : morpho.rootSections())
        {
            std::stack<geometry::NodePtr> prevNodes;
            std::stack<morphio::Section> currentSections;

            for (auto section : morpho.rootSections())
            {
                prevNodes.push(nullptr);
                currentSections.push(section);
            }

            uint32_t nodeId = 0;

            while (!currentSections.empty())
            {
                auto currentSection = currentSections.top();
                currentSections.pop();
                auto section = new examples::Section();
                sections.push_back(section);
                auto prevNode = prevNodes.top();
                prevNodes.pop();

                uint32_t i = 0;
                if (prevNode)
                {
                    i = 1;
                    section->push_back(prevNode);
                }
                geometry::NodePtr node = prevNode;
                for (; i < currentSection.points().size(); ++i)
                {
                    auto p = currentSection.points()[i];
                    geometry::Vec4 position(p[0], p[1], p[2], 1);
                    position = mat * position;

                    float radius = currentSection.diameters()[i] * 0.5;
                    node = new geometry::Node(position, nodeId, radius,
                                              geometry::Vec3(),
                                              geometry::Vec3(), radius);
                    ++nodeId;
                    nodes.push_back(node);
                    section->push_back(node);
                }

                for (auto child : currentSection.children())
                {
                    currentSections.push(child);
                    prevNodes.push(node);
                }
            }
        }

        for (auto section : sections)
        {
            for (uint32_t i = 1; i < section->size(); ++i)
                edges.push_back(
                    new geometry::Edge((*section)[i - 1], (*section)[i]));
            delete section;
        }
        sections.clear();
    }

    for (auto section : morpho.rootSections())
    {
        auto p = section.points()[0];
        geometry::Vec4 pos4(p[0], p[1], p[2], 1);
        pos4 = mat * pos4;
        geometry::Vec3 pos(pos4);
        float radius = section.diameters()[0];
        sectionNodes.push_back(new geometry::Node(
            pos, 0, radius, geometry::Vec3(), geometry::Vec3(), radius));
    }

    for (uint32_t i = 0; i < morpho.soma().points().size(); ++i)
    {
        auto p = morpho.soma().points()[i];
        geometry::Vec4 pos4(p[0], p[1], p[2], 1);
        pos4 = mat * pos4;
        geometry::Vec3 pos(pos4);
        float radius = morpho.soma().diameters()[i];
        somaNodes.push_back(new geometry::Node(pos, 0, radius, geometry::Vec3(),
                                               geometry::Vec3(), radius));
    }

    morphio::Point c = morpho.soma().center();
    geometry::Vec4 center4(c[0], c[1], c[2], 1);
    center4 = mat * center4;
    geometry::Vec3 center(center4);

    float radius = 0.0;

    switch (radiusFunc)
    {
    case MIN_NEURITES:
        radius = std::numeric_limits<float>::max();
        for (auto node : sectionNodes)
        {
            auto dist = glm::distance(node->position, center);
            if (dist < radius) radius = dist;
        }
        break;
    case MAX_NEURITES:
        for (auto node : sectionNodes)
        {
            auto dist = glm::distance(node->position, center);
            if (dist > radius) radius = dist;
        }
        break;
    case MEAN_NEURITES:
        for (auto node : sectionNodes)
            radius += glm::distance(node->position, center);
        radius /= morpho.rootSections().size();
        break;
    case MIN_SOMAS:
        radius = std::numeric_limits<float>::max();
        for (auto node : somaNodes)
        {
            auto dist = glm::distance(node->position, center);
            if (dist < radius) radius = dist;
        }
        break;
    case MAX_SOMAS:
        for (auto node : somaNodes)
        {
            auto dist = glm::distance(node->position, center);
            if (dist > radius) radius = dist;
        }
        break;
    case MEAN_SOMAS:
        for (auto node : somaNodes)
            radius += glm::distance(node->position, center);
        radius /= somaNodes.size();
        break;
    }

    soma = new geometry::Node(center, 0, radius, geometry::Vec3(),
                              geometry::Vec3(), radius);
    soma->isSoma = true;

    // nodes.push_back(soma);
    // edges.push_back(new geometry::Edge(soma, soma));

    aabb = new geometry::HierarchicalAABB(edges);
#endif
}

void Morpho::cutout(geometry::AxisAlignedBoundingBox& other)
{
    auto colEdges = aabb->collidingEdges(other);
    auto colNodes = geometry::uniqueNodes(colEdges);

    std::unordered_set<geometry::Edge*> uEdges(colEdges.begin(),
                                               colEdges.end());
    std::unordered_set<geometry::NodePtr> uNodes(colNodes.begin(),
                                                 colNodes.end());

    if (!other.isColliding(*soma))
    {
        for (auto node : somaNodes) delete node;
        somaNodes.clear();
        soma = nullptr;
    }

    for (auto edge : edges)
        if (uEdges.find(edge) == uEdges.end()) delete edge;
    edges = colEdges;

    for (auto node : nodes)
        if (uNodes.find(node) == uNodes.end()) delete node;
    nodes = colNodes;

    delete aabb;
    aabb = new geometry::HierarchicalAABB(edges);
}

void Morpho::print()
{
    for (auto section : sections)
    {
        std::cout << "Section" << std::endl;

        auto node = section->at(0);
        std::cout << "\tNode " << node->id << " position " << node->position.x
                  << " " << node->position.y << " " << node->position.z
                  << " radius " << node->radius << std::endl;

        node = section->at(section->size() - 1);
        std::cout << "\tNode " << node->id << " position " << node->position.x
                  << " " << node->position.y << " " << node->position.z
                  << " radius " << node->radius << std::endl;

        // for (auto node : *section)
        // {
        //     std::cout << "node " << node->id << " pos " <<
        //     node->position.x()
        //               << " " << node->position.y() << " " <<
        //               node->position.z()
        //               << " r " << node->radius << std::endl;
        // }
    }
}

}  // namespace examples