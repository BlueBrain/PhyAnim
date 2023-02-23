#ifndef __EXAMPLES_OVERLAP_CIRCUIT_APP__
#define __EXAMPLES_OVERLAP_CIRCUIT_APP__

#include "../common/CollisionSolver.h"
#include "../common/ColorPalette.h"
#include "../common/GLFWApp.h"
#include "../common/Morpho.h"

namespace examples
{
class OverlapCircuitApp : public GLFWApp
{
public:
    OverlapCircuitApp(int argc, char** argv);

protected:
    bool _solveCollisions(phyanim::HierarchicalAABBs& aabbs,
                          std::vector<phyanim::Edges>& edgesSet,
                          std::vector<phyanim::Nodes>& nodesSet,
                          phyanim::AxisAlignedBoundingBox& limits,
                          double ks,
                          double ksc,
                          double kd,
                          uint32_t numIters = 1000);

    void _actionLoop();

    void _setMeshes(std::vector<phyanim::Edges>& edgesSet);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

private:
    ColorPalette* _palette;

    CollisionSolver* _solver;
};

}  // namespace examples

#endif
