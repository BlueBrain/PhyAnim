#ifndef __EXAMPLES_OVERLAP_COLLISIONS_APP__
#define __EXAMPLES_OVERLAP_COLLISIONS_APP__

#include <GLFWApp.h>

#include <chrono>

namespace examples
{
class OverlapCollisionsApp : public GLFWApp
{
public:
    OverlapCollisionsApp(int argc, char** argv);

protected:
    void _actionLoop();

    phyanim::MeshPtr _sliceMesh(phyanim::MeshPtr mesh,
                                const phyanim::AxisAlignedBoundingBox& aabb,
                                double stiffness,
                                bool density,
                                bool damping,
                                bool poissonRatio);

    void _setSurfaceNodes(phyanim::MeshPtr mesh);
};

}  // namespace examples

#endif
