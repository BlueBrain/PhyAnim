#ifndef __EXAMPLES_SOMA_APP__
#define __EXAMPLES_SOMA_APP__

#include <GLFWApp.h>

#include "SomaGenerator.h"

namespace examples
{
class SomaApp : public GLFWApp
{
public:
    SomaApp();

    void init(int argc, char** argv);

    void loop();

    SomaGenerator* somaGen;

protected:
    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

    bool _anim;

    double _dt;
    double _ks;
    uint16_t _iters;
    uint16_t _iter;
};

}  // namespace examples

#endif
