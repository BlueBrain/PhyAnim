#ifndef __EXAMPLES_SOMA_GENERATOR__
#define __EXAMPLES_SOMA_GENERATOR__

#include "Icosphere.h"

using namespace phyanim;

namespace examples
{
class SomaGenerator
{
public:
    SomaGenerator(geometry::Vec3 center,
                  float radius,
                  geometry::Nodes starts,
                  float dt = 0.01,
                  float stiffness = 1000.0,
                  float poissonRatio = 0.49);

    ~SomaGenerator(){};

    void anim(bool updateNodes = false);

    void pull(float alpha);

    geometry::MeshPtr animMesh;
    graphics::Mesh* renderMesh;

private:
    void _computeStartsNodes();

    void _fixCenterNodes(float radialDist = 0.5);

    void _updateNodes();

    geometry::Vec3 _center;
    float _radius;

    geometry::Nodes _starts;
    std::vector<geometry::Nodes> _startsNodes;
    std::vector<geometry::Vec3> _targets;
    float _dt;

    anim::ImplicitFEMSystem* _sys;
};

}  // namespace examples

#endif
