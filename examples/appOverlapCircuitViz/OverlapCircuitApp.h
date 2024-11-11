#ifndef __EXAMPLES_OVERLAP_CIRCUIT_APP__
#define __EXAMPLES_OVERLAP_CIRCUIT_APP__

#include "../common/CollisionSolver.h"
#include "../common/GLFWApp.h"
#include "../common/Morpho.h"

namespace examples
{
class OverlapCircuitApp : public GLFWApp
{
public:
    OverlapCircuitApp(int argc, char** argv);

protected:
    void _actionLoop();

    uint32_t _solveCollisions(phyanim::geometry::HierarchicalAABBs& aabbs,
                              std::vector<phyanim::geometry::Edges>& edgesSet,
                              std::vector<phyanim::geometry::Nodes>& nodesSet,
                              phyanim::geometry::AxisAlignedBoundingBox& limits,
                              uint32_t& totalIters);

    void _setMeshes(std::vector<phyanim::geometry::Edges>& edgesSet);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

private:
    phyanim::graphics::ColorPalette* _palette;

    CollisionSolver* _solver;

    float _threshold;
    float _ks;
    float _ksc;
    float _ksLimit;
    float _dt;
};

}  // namespace examples

#endif
