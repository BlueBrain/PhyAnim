#ifndef __EXAMPLES_RENDER_MORPHO_APP__
#define __EXAMPLES_RENDER_MORPHO_APP__

#include "../common/GLFWApp.h"
#include "../common/Morpho.h"

namespace examples
{
class RenderMorphoApp : public GLFWApp
{
public:
    RenderMorphoApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

    std::vector<Morpho*> _morphologies;
};

}  // namespace examples

#endif
