#include "Morpho.h"

#ifdef PHYANIM_USES_MORPHO
#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

#endif

#include <stack>

namespace examples
{

Morpho::Morpho(std::string path, phyanim::Mat4 mat)
    : color(0.2, 0.8, 0.2)
    , collColor(1.0, 0.0, 0.0)
    , fixColor(0.2, 0.2, 0.2)
{
#ifdef PHYANIM_USES_MORPHO
    auto morpho = morphio::Morphology(path);

    for (auto section : morpho.rootSections())
    {
        std::stack<phyanim::NodePtr> prevNodes;
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
            phyanim::NodePtr node = prevNode;
            for (; i < currentSection.points().size(); ++i)
            {
                auto p = currentSection.points()[i];
                phyanim::Vec4 position(p[0], p[1], p[2], 1);
                position = mat * position;

                double radius = currentSection.diameters()[i] * 0.5;
                node = new phyanim::Node(position.head<3>(), nodeId, radius,
                                         phyanim::Vec3::Zero(),
                                         phyanim::Vec3::Zero(), radius);
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
                new phyanim::Edge((*section)[i - 1], (*section)[i]));
        delete section;
    }
    sections.clear();

    morphio::Point c = morpho.soma().center();
    phyanim::Vec4 center4(c[0], c[1], c[2], 1);
    center4 = mat * center4;
    phyanim::Vec3 center = center4.head<3>();
    double radius = std::numeric_limits<double>::max();
    for (auto section : morpho.rootSections())
    {
        morphio::Point p = section.points()[0];
        phyanim::Vec4 pos4(p[0], p[1], p[2], 1);
        pos4 = mat * pos4;
        phyanim::Vec3 pos = pos4.head<3>();
        double dist = (pos - center).norm();
        if (dist < radius) radius = dist;
    }
    auto node = new phyanim::Node(center, 0, radius, phyanim::Vec3::Zero(),
                                  phyanim::Vec3::Zero(), radius);
    node->isSoma = true;
    nodes.push_back(node);
    edges.push_back(new phyanim::Edge(node, node));

    aabb = new phyanim::HierarchicalAABB(edges);
#endif
}

void Morpho::cutout(phyanim::AxisAlignedBoundingBox& other)
{
    auto colEdges = aabb->collidingEdges(other);
    auto colNodes = phyanim::uniqueNodes(colEdges);

    std::unordered_set<phyanim::Edge*> uEdges(colEdges.begin(), colEdges.end());
    std::unordered_set<phyanim::NodePtr> uNodes(colNodes.begin(),
                                                colNodes.end());

    for (auto edge : edges)
        if (uEdges.find(edge) == uEdges.end()) delete edge;
    edges = colEdges;

    for (auto node : nodes)
        if (uNodes.find(node) == uNodes.end()) delete node;
    nodes = colNodes;

    delete aabb;
    aabb = new phyanim::HierarchicalAABB(edges);
}

void Morpho::print()
{
    for (auto section : sections)
    {
        std::cout << "Section" << std::endl;

        auto node = section->at(0);
        std::cout << "\tNode " << node->id << " position " << node->position.x()
                  << " " << node->position.y() << " " << node->position.z()
                  << " radius " << node->radius << std::endl;

        node = section->at(section->size() - 1);
        std::cout << "\tNode " << node->id << " position " << node->position.x()
                  << " " << node->position.y() << " " << node->position.z()
                  << " radius " << node->radius << std::endl;

        // for (auto node : *section)
        // {
        //     std::cout << "node " << node->id << " pos " << node->position.x()
        //               << " " << node->position.y() << " " <<
        //               node->position.z()
        //               << " r " << node->radius << std::endl;
        // }
    }
}

}  // namespace examples