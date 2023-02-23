#ifndef __EXAMPLES_OVERLAP_COLLISIONS_APP__
#define __EXAMPLES_OVERLAP_COLLISIONS_APP__

#include <chrono>

#include "../common/GLFWApp.h"

namespace examples
{
class OverlapCollisionsApp : public GLFWApp
{
public:
    OverlapCollisionsApp(int argc, char** argv);

protected:
    void _actionLoop();

    phyanim::MeshPtr _sliceMesh(phyanim::HierarchicalAABBPtr tetAABB,
                                const phyanim::AxisAlignedBoundingBox& aabb,
                                double stiffness,
                                double density,
                                double damping,
                                double poissonRatio);

    void _setSurfaceNodes(phyanim::MeshPtr mesh);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);
};

}  // namespace examples

#endif
