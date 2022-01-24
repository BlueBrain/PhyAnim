#include "Icosphere.h"

#include <Tetrahedron.h>
#include <Triangle.h>

#include <iostream>
#include <unordered_set>

namespace examples
{
Icosphere::Icosphere(phyanim::Vec3 pos, double radius)
{
    _mesh = new phyanim::DrawableMesh();
    _mesh->load(PATH_TO_ICO);
    for (auto meshNode : _mesh->nodes)
    {
        phyanim::Vec3 position = meshNode->position * radius + pos;
        auto node = new Node(position, meshNode->id);
        nodes.push_back(node);
    }
}

phyanim::DrawableMesh* Icosphere::mesh() { return _mesh; }

std::vector<Spring*> Icosphere::springs(double stiffness)
{
    UniqueSprings uSprings;

    for (auto primitive : _mesh->triangles)
    {
        auto triangle = dynamic_cast<phyanim::TrianglePtr>(primitive);
        auto node0 = nodes[triangle->node0->id];
        auto node1 = nodes[triangle->node1->id];
        auto node2 = nodes[triangle->node2->id];
        _insert(new Spring(node0, node1, stiffness), uSprings);
        _insert(new Spring(node0, node2, stiffness), uSprings);
        _insert(new Spring(node1, node2, stiffness), uSprings);
    }
    std::vector<Spring*> springs(uSprings.begin(), uSprings.end());
    return springs;
}

std::vector<Spring*> Icosphere::surfaceSprings(double stiffness)
{
    UniqueSprings uSprings;

    for (auto primitive : _mesh->surfaceTriangles)
    {
        auto triangle = dynamic_cast<phyanim::TrianglePtr>(primitive);
        auto node0 = nodes[triangle->node0->id];
        auto node1 = nodes[triangle->node1->id];
        auto node2 = nodes[triangle->node2->id];
        _insert(new Spring(node0, node1, stiffness), uSprings);
        _insert(new Spring(node0, node2, stiffness), uSprings);
        _insert(new Spring(node1, node2, stiffness), uSprings);
    }
    std::vector<Spring*> springs(uSprings.begin(), uSprings.end());
    return springs;
}

Tets Icosphere::tets()
{
    Tets tets;
    for (auto primitive : _mesh->tetrahedra)
    {
        auto t = dynamic_cast<phyanim::TetrahedronPtr>(primitive);
        auto tet = new Tet(nodes[t->node0->id], nodes[t->node1->id],
                           nodes[t->node2->id], nodes[t->node3->id]);
        tets.push_back(tet);
    }
    return tets;
}

void Icosphere::_insert(Spring* spring, UniqueSprings& springs)
{
    std::pair<UniqueSprings::iterator, bool> insertion = springs.insert(spring);
    if (!insertion.second) delete spring;
}

}  // namespace examples
