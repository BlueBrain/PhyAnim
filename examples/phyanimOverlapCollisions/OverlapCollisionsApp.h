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
