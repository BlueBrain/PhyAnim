#ifndef __EXAMPLES_MORPHO_H
#define __EXAMPLES_MORPHO_H

#include <Phyanim.h>

#include "Mesh.h"

namespace examples
{

typedef phyanim::Nodes Section;
typedef std::vector<Section*> Sections;

class Morpho
{
public:
    Morpho(std::string path, phyanim::Mat4 mat = phyanim::Mat4::Identity());

    ~Morpho(){};

    void print();

    void cutout(phyanim::AxisAlignedBoundingBox& aabb);

    // phyanim::Nodes somaNodes;

    phyanim::Nodes nodes;

    Sections sections;

    phyanim::HierarchicalAABBPtr aabb;

    phyanim::Edges edges;

    phyanim::Vec3 color;

    phyanim::Vec3 collColor;

    phyanim::Vec3 fixColor;
};

}  // namespace examples

#endif
