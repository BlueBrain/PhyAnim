#ifndef __EXAMPLES_ICOSPHERE__
#define __EXAMPLES_ICOSPHERE__

#include <DrawableMesh.h>

#include "Spring.h"

namespace examples
{
class Icosphere
{
public:
    Icosphere(phyanim::Vec3 pos = phyanim::Vec3::Zero(), double radius = 1.0f);

    virtual ~Icosphere(){};

    phyanim::DrawableMesh* mesh();

    std::vector<Spring*> springs(double stiffness = 100.0);

    std::vector<Node*> nodes;

    std::vector<Triangle*> triangles;

private:
    phyanim::DrawableMesh* _mesh;

    void _insert(Spring* spring, UniqueSprings& springs);

    static const uint16_t _verticesSize;

    static const double _vertices[];

    static const uint16_t _indicesSize;

    static const uint16_t _indices[];
};

}  // namespace examples
#endif
