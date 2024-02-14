#ifndef __EXAMPLES_MORPHO_H
#define __EXAMPLES_MORPHO_H

#include <phyanim/Phyanim.h>

using namespace phyanim;

namespace examples
{
typedef geometry::Nodes Section;
typedef std::vector<Section*> Sections;

enum RadiusFunc
{
    MIN_NEURITES,
    MAX_NEURITES,
    MEAN_NEURITES,
    MIN_SOMAS,
    MAX_SOMAS,
    MEAN_SOMAS
};

class Morpho
{
public:
    Morpho(std::string path,
           geometry::Mat4 mat = geometry::Mat4(1.0f),
           RadiusFunc radiusFunc = RadiusFunc::MAX_NEURITES,
           bool loadNeurites = true);

    ~Morpho(){};

    void print();

    void cutout(geometry::AxisAlignedBoundingBox& aabb);

    geometry::Nodes nodes;

    geometry::Nodes somaNodes;

    geometry::NodePtr soma;

    geometry::Nodes sectionNodes;

    Sections sections;

    geometry::HierarchicalAABBPtr aabb;

    geometry::Edges edges;

    geometry::Vec3 color;

    geometry::Vec3 collColor;

    geometry::Vec3 fixColor;
};

}  // namespace examples

#endif
