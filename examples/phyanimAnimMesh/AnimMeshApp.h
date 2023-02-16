#ifndef __EXAMPLES_ANIM_MESH_APP__
#define __EXAMPLES_ANIM_MESH_APP__

#include "../common/GLFWApp.h"

namespace examples
{
class AnimMeshApp : public GLFWApp
{
public:
    AnimMeshApp(int argc, char** argv) : GLFWApp(argc, argv){};

protected:
    void _actionLoop();
};

}  // namespace examples

#endif
