#include "SomaGenerator.h"

#include <iostream>

namespace examples
{
SomaGenerator::SomaGenerator(NeuriteStarts starts,
                             phyanim::Vec3 pos,
                             double radius,
                             double dt,
                             double stiffness,
                             double fixedThreshold)
    : _center(pos)
    , _radius(radius)
    , _dt(dt)
{
    _stiffness = (1 / (_dt / (2 * M_PI))) * 0.01 * stiffness;
    _pullStiffness = 0;

    _ico = new Icosphere(_center, _radius);

    _nodes = _ico->nodes;
    _mesh = _ico->mesh();

    _springs = _ico->springs(_stiffness);
    _computePullSprings(starts, _stiffness, 0.01);
}

void SomaGenerator::simulate(uint64_t iters)
{
    double incStiffness = _stiffness / iters;
    for (uint64_t i = 0; i < iters; ++i)
    {
        _pullStiffness += incStiffness;
        anim(false);
    }
    _updateNodes();
}

void SomaGenerator::anim(bool updateNodes)
{
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < _nodes.size(); ++i)
    {
        _nodes[i]->force = phyanim::Vec3::Zero();
    }
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < _springs.size(); ++i)
    {
        _computeForce(_springs[i]);
    }
    for (auto spring : _springs)
    {
        spring->node0->force += spring->force;
        spring->node1->force -= spring->force;
    }

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < _pullSprings.size(); ++i)
    {
        _pullSprings[i]->stiffness = _pullStiffness;
        _computeForce(_pullSprings[i]);
    }
    for (auto spring : _pullSprings)
    {
        spring->node0->force += spring->force;
        spring->node1->force -= spring->force;
    }
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < _nodes.size(); ++i)
    {
        auto node = _nodes[i];
        if (!node->fixed)
        {
            phyanim::Vec3 v = node->velocity + node->force / node->mass * _dt;
            phyanim::Vec3 x = node->position + v * _dt;

            node->velocity = v;
            node->position = x;
        }
    }

    if (updateNodes) _updateNodes();
}

phyanim::DrawableMesh* SomaGenerator::mesh() { return _mesh; }

void SomaGenerator::_computePullSprings(NeuriteStarts starts,
                                        double stiffness,
                                        double resThreshold)
{
    for (auto start : starts)
    {
        phyanim::Vec3 startPos = start.first;
        double startRadius = start.second;

        phyanim::Vec3 direction = (startPos - _center).normalized();
        phyanim::Vec3 surfacePos = direction * _radius + _center;
        double diff = (startPos - surfacePos).norm();
        if (diff < 0.1)
        {
            diff = 0.1;
        }
        direction *= diff;

        for (auto node : _nodes)
        {
            if ((node->position - surfacePos).norm() < startRadius)
            {
                auto linkNode = new Node(node->position + direction, 0, true);
                _pullNodes.push_back(linkNode);
                _pullSprings.push_back(
                    new Spring(node, linkNode, stiffness, diff * resThreshold));
            }
        }
    }
}

void SomaGenerator::_innerSprings(double stiffness)
{
    _centerNode = new Node(_center, 0, true);
    for (auto node : _nodes)
        _springs.push_back(new Spring(node, _centerNode, stiffness));
}

void SomaGenerator::_fixCenterNodes(double threshold)
{
    for (auto node : _nodes)
    {
        node->fixed = ((node->position - _center).norm() < _radius * threshold);
    }
}

void SomaGenerator::_computeForce(SpringPtr spring)
{
    double ks = spring->stiffness;
    double kd = ks * 0.1;
    double m = (spring->node0->mass + spring->node1->mass) * 0.5;
    phyanim::Vec3 d = spring->node1->position - spring->node0->position;
    double r = spring->resLength;
    double l = d.norm();
    phyanim::Vec3 v = spring->node1->velocity - spring->node0->velocity;
    phyanim::Vec3 f0 = phyanim::Vec3::Zero();
    if (l > 0.0)
    {
        f0 = (ks * (l / r - 1.0) + kd * (v.dot(d) / (l * r))) * d / l;
    }
    spring->force = f0;
}

void SomaGenerator::_updateNodes()
{
    for (uint64_t i = 0; i < _mesh->nodes.size(); ++i)
        _mesh->nodes[i]->position = _ico->nodes[i]->position;
    _mesh->updatedPositions = true;
}

}  // namespace examples
