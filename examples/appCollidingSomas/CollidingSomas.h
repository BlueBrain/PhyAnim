#ifndef __EXAMPLES_COLLIDING_SOMAS__
#define __EXAMPLES_COLLIDING_SOMAS__

#include "../common/CollisionSolver.h"
#include "../common/ColorPalette.h"
#include "../common/GLFWApp.h"
#include "../common/Morpho.h"

namespace examples
{
class CollidingSomas : public GLFWApp
{
public:
    CollidingSomas(int argc, char** argv);

    ~CollidingSomas(){};

protected:
    void _actionLoop();

    void _setMeshes(std::vector<phyanim::Edges>& edgesSet,
                    std::vector<double> factor);

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
