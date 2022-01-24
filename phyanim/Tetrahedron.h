#ifndef __PHYANIM_TETRAHEDRON__
#define __PHYANIM_TETRAHEDRON__

#include <Edge.h>
#include <Triangle.h>

namespace phyanim
{
class Tetrahedron;

typedef Tetrahedron* TetrahedronPtr;

typedef std::vector<TetrahedronPtr> Tetrahedra;

class Tetrahedron : public Primitive
{
public:
    Tetrahedron(Node* n0_, Node* n1_, Node* n2_, Node* n3_);

    ~Tetrahedron() {}

    Node* node0;

    Node* node1;

    Node* node2;

    Node* node3;

    double initVolume();

    double volume() const;

    Nodes nodes() const;

    Edges edges() const;

    Triangles triangles();

private:
    double _volume;
    bool _volumeComputed;
};

}  // namespace phyanim

#endif
