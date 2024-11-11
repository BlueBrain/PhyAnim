#ifndef __EXAMPLES_COLLIDING_SOMAS__
#define __EXAMPLES_COLLIDING_SOMAS__

#include "../common/CollisionSolver.h"
#include "../common/GLFWApp.h"
#include "../common/Morpho.h"

using namespace phyanim;

namespace examples
{
class CollidingSomas : public GLFWApp
{
public:
    CollidingSomas(int argc, char** argv);

    ~CollidingSomas(){};

protected:
    void _actionLoop();

    void _setMeshes(std::vector<geometry::Edges>& edgesSet);

    uint32_t _solveCollisions(geometry::HierarchicalAABBs& aabbs,
                              std::vector<geometry::Edges>& edgesSet,
                              std::vector<geometry::Nodes>& nodesSet);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

private:
    graphics::ColorPalette* _palette;

    CollisionSolver* _solver;

    float _dt;
    float _ksc;
};

}  // namespace examples

#endif
