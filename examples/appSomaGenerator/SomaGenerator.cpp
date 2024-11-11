#include "SomaGenerator.h"

#include <iostream>
#include <thread>

namespace examples
{
SomaGenerator::SomaGenerator(geometry::Vec3 center,
                             float radius,
                             geometry::Nodes starts,
                             float dt,
                             float stiffness,
                             float poissonRatio)
    : _center(center)
    , _radius(radius)
    , _starts(starts)
    , _dt(dt)
{
    animMesh = new Icosphere(_center, _radius);
    animMesh->stiffness = stiffness;
    animMesh->poissonRatio = poissonRatio;
    renderMesh =
        graphics::generateMesh(animMesh->nodes, animMesh->surfaceTriangles);

    _updateNodes();
    _computeStartsNodes();

    _sys = new anim::ImplicitFEMSystem(_dt);
    _sys->preprocessMesh(animMesh);
    _sys->gravity = false;
}

void SomaGenerator::anim(bool updateNodes)
{
    _sys->step(animMesh);

    if (updateNodes) _updateNodes();
}

void SomaGenerator::pull(float alpha)
{
    for (uint64_t i = 0; i < _starts.size(); ++i)
    {
        geometry::Vec3 increment = (_starts[i]->position - _targets[i]) * alpha;
        for (auto node : _startsNodes[i])
        {
            node->position = node->initPosition + increment;
        }
    }
}

void SomaGenerator::_computeStartsNodes()
{
    _targets.resize(_starts.size());
    _startsNodes.resize(_starts.size());

    std::set<geometry::NodePtr> surfaceNodesSet;
    for (auto node : animMesh->nodes) surfaceNodesSet.insert(node);
    geometry::Nodes surfaceNodes(surfaceNodesSet.begin(),
                                 surfaceNodesSet.end());

    std::vector<bool> assignedNodes(surfaceNodes.size());
    for (auto assignedNode : assignedNodes) assignedNode = false;

    for (uint64_t i = 0; i < _starts.size(); ++i)
    {
        auto start = _starts[i];
        geometry::Vec3 direction = glm::normalize(start->position - _center);
        geometry::Vec3 surfacePos = direction * _radius + _center;
        _targets[i] = surfacePos;

        float minDist = std::numeric_limits<float>::max();
        uint64_t minId = 0;
        for (uint64_t nodeId = 0; nodeId < surfaceNodes.size(); ++nodeId)
        {
            if (!assignedNodes[nodeId])
            {
                auto node = surfaceNodes[nodeId];
                float dist = glm::distance(node->position, surfacePos);

                if (dist < minDist)
                {
                    minDist = dist;
                    minId = nodeId;
                }

                if (dist <= start->radius)
                {
                    _startsNodes[i].push_back(node);
                    node->fix = true;
                    assignedNodes[nodeId] = true;
                }
            }
        }

        if (_startsNodes[i].empty())
        {
            auto node = surfaceNodes[minId];
            _startsNodes[i].push_back(node);
            _targets[i] = node->position;
            node->fix = true;
            assignedNodes[minId] = true;
        }
    }
}

void SomaGenerator::_fixCenterNodes(float radialDist)
{
    for (auto node : animMesh->nodes)
    {
        float dist = glm::distance(node->position, _center);
        if (dist < radialDist * _radius) node->fix = true;
    }
}

void SomaGenerator::_updateNodes()
{
    animMesh->computeNormals();
    setGeometry(renderMesh, animMesh->nodes);
}

}  // namespace examples
