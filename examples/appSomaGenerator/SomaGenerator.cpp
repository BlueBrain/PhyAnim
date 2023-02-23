#include "SomaGenerator.h"

#include <iostream>

namespace examples
{
SomaGenerator::SomaGenerator(Samples starts,
                             Sample soma,
                             double dt,
                             double stiffness,
                             double poissonRatio,
                             double alphaSoma)
    : _starts(starts)
    , _soma(soma)
    , _dt(dt)
{
    _soma.radius *= alphaSoma;
    _ico = new Icosphere(_soma.position, _soma.radius);
    _nodes = _ico->nodes;
    _animMesh = _ico->mesh();
    _renderMesh = generateMesh(_animMesh->nodes, _animMesh->surfaceTriangles);
    _tets = _ico->tets();
    _updateNodes();

    _kMat.computeMatrices(_nodes, _tets, stiffness, poissonRatio, _dt);
    _computeStartsNodes();
}

void SomaGenerator::anim(bool updateNodes)
{
    // Solve implicit system
    uint64_t size = _nodes.size() * 3;
    Eigen::VectorXd u(size);
    Eigen::VectorXd mv(size);
    Eigen::VectorXd v_1(size);
    Eigen::VectorXd b(size);
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < _nodes.size(); ++i)
    {
        auto node = _nodes[i];
        _addVec3ToVec(i, node->position - node->initPosition, u);
        _addVec3ToVec(i, node->velocity * node->mass, mv);
    }

    b = mv - _dt * (_kMat.kMatrix * u);
    v_1 = _kMat.aMatrixSolver.solve(b);

    // Update nodes velocity and position
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < _nodes.size(); ++i)
    {
        auto node = _nodes[i];
        if (!node->fixed)
        {
            node->velocity =
                phyanim::Vec3(v_1[i * 3], v_1[i * 3 + 1], v_1[i * 3 + 2]);
            node->position += node->velocity * _dt;
        }
    }

    if (updateNodes) _updateNodes();
}

void SomaGenerator::pull(float alpha)
{
    for (uint64_t i = 0; i < _starts.size(); ++i)
    {
        phyanim::Vec3 increment =
            (_starts[i]->position - _positions[i]) * alpha;
        for (auto node : _startsNodes[i])
            node->position = node->initPosition + increment;
    }
}

void SomaGenerator::_addVec3ToVec(uint64_t id,
                                  const phyanim::Vec3& value,
                                  Eigen::VectorXd& vec)
{
    id *= 3;
    for (uint64_t i = 0; i < 3; ++i)
    {
        vec[id + i] = value[i];
    }
}

void SomaGenerator::_computeStartsNodes()
{
    _startsNodes.resize(_starts.size());
    _positions.resize(_starts.size());

    std::vector<bool> assignedNodes(_ico->surfaceNodes.size());
    for (auto assignedNode : assignedNodes) assignedNode = false;

    for (uint64_t i = 0; i < _starts.size(); ++i)
    {
        auto start = _starts[i];
        phyanim::Vec3 direction =
            (start->position - _soma.position).normalized();
        phyanim::Vec3 surfacePos = direction * _soma.radius + _soma.position;
        _positions[i] = surfacePos;

        float minDist = std::numeric_limits<double>::max();
        uint64_t minId = 0;
        for (uint64_t nodeId = 0; nodeId < _ico->surfaceNodes.size(); ++nodeId)
        {
            if (!assignedNodes[nodeId])
            {
                auto node = _ico->surfaceNodes[nodeId];
                double dist = (node->position - surfacePos).norm();

                if (dist < minDist)
                {
                    minDist = dist;
                    minId = nodeId;
                }

                if (dist <= start->radius)
                {
                    _startsNodes[i].push_back(node);
                    node->fixed = true;
                    assignedNodes[nodeId] = true;
                }
            }
        }

        if (_startsNodes[i].empty())
        {
            auto node = _ico->surfaceNodes[minId];
            _startsNodes[i].push_back(node);
            _positions[i] = node->position;
            node->fixed = true;
            assignedNodes[minId] = true;
        }
    }
}

void SomaGenerator::_fixCenterNodes(double radialDist)
{
    for (auto node : _nodes)
    {
        double dist = (node->position - _soma.position).norm();
        if (dist < radialDist * _soma.radius) node->fixed = true;
    }
}

void SomaGenerator::_updateNodes()
{
    for (uint64_t i = 0; i < _animMesh->nodes.size(); ++i)
        _animMesh->nodes[i]->position = _ico->nodes[i]->position;
    _animMesh->computeNormals();
    setGeometry(_renderMesh, _animMesh->nodes);
}

}  // namespace examples
