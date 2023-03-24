#include "Morpho.h"

#ifdef PHYANIM_USES_MORPHO
#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

#endif

#include <stack>

namespace examples
{

Morpho::Morpho(std::string path,
               phyanim::Mat4 mat,
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
    }

    for (auto section : morpho.rootSections())
    {
        auto p = section.points()[0];
        phyanim::Vec4 pos4(p[0], p[1], p[2], 1);
        pos4 = mat * pos4;
        phyanim::Vec3 pos = pos4.head<3>();
        double radius = section.diameters()[0];
        sectionNodes.push_back(
            new phyanim::Node(pos, 0, radius, phyanim::Vec3::Zero(),
                              phyanim::Vec3::Zero(), radius));
    }

    for (uint32_t i = 0; i < morpho.soma().points().size(); ++i)
    {
        auto p = morpho.soma().points()[i];
        phyanim::Vec4 pos4(p[0], p[1], p[2], 1);
        pos4 = mat * pos4;
        phyanim::Vec3 pos = pos4.head<3>();
        double radius = morpho.soma().diameters()[i];
        somaNodes.push_back(new phyanim::Node(pos, 0, radius,
                                              phyanim::Vec3::Zero(),
                                              phyanim::Vec3::Zero(), radius));
    }

    morphio::Point c = morpho.soma().center();
    phyanim::Vec4 center4(c[0], c[1], c[2], 1);
    center4 = mat * center4;
    phyanim::Vec3 center = center4.head<3>();

    double radius = 0.0;

    switch (radiusFunc)
    {
    case MIN_NEURITES:
        radius = std::numeric_limits<double>::max();
        for (auto node : sectionNodes)
        {
            auto dist = (node->position - center).norm();
            if (dist < radius) radius = dist;
        }
        break;
    case MAX_NEURITES:
        for (auto node : sectionNodes)
        {
            auto dist = (node->position - center).norm();
            if (dist > radius) radius = dist;
        }
        break;
    case MEAN_NEURITES:
        for (auto node : sectionNodes)
            radius += (node->position - center).norm();
        radius /= morpho.rootSections().size();
        break;
    case MIN_SOMAS:
        radius = std::numeric_limits<double>::max();
        for (auto node : somaNodes)
        {
            auto dist = (node->position - center).norm();
            if (dist < radius) radius = dist;
        }
        break;
    case MAX_SOMAS:
        for (auto node : somaNodes)
        {
            auto dist = (node->position - center).norm();
            if (dist > radius) radius = dist;
        }
        break;
    case MEAN_SOMAS:
        for (auto node : somaNodes) radius += (node->position - center).norm();
        radius /= somaNodes.size();
        break;
    }

    soma = new phyanim::Node(center, 0, radius, phyanim::Vec3::Zero(),
                             phyanim::Vec3::Zero(), radius);
    soma->isSoma = true;

    // nodes.push_back(soma);
    // edges.push_back(new phyanim::Edge(soma, soma));

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
        //     std::cout << "node " << node->id << " pos " <<
        //     node->position.x()
        //               << " " << node->position.y() << " " <<
        //               node->position.z()
        //               << " r " << node->radius << std::endl;
        // }
    }
}

}  // namespace examples