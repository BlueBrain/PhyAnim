#ifndef __EXAMPLES_OVERLAP_COLLISIONS_APP__
#define __EXAMPLES_OVERLAP_COLLISIONS_APP__

#include <chrono>

#include "../common/GLFWApp.h"

using namespace phyanim;

namespace examples
{
class OverlapCollisionsApp : public GLFWApp
{
public:
    OverlapCollisionsApp(int argc, char** argv);

protected:
    void _actionLoop();

    geometry::MeshPtr _sliceMesh(geometry::HierarchicalAABBPtr tetAABB,
                                 const geometry::AxisAlignedBoundingBox& aabb,
                                 float stiffness,
                                 float density,
                                 float damping,
                                 float poissonRatio);

    void _setSurfaceNodes(geometry::MeshPtr mesh);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);
};

}  // namespace examples

#endif
