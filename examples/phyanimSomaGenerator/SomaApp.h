#ifndef __EXAMPLES_SOMA_APP__
#define __EXAMPLES_SOMA_APP__

#include "../common/GLFWApp.h"
#include "SomaGenerator.h"

namespace examples
{
class SomaApp : public GLFWApp
{
public:
    SomaApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

    bool _anim;

    std::mutex _animMutex;
};

}  // namespace examples

#endif
