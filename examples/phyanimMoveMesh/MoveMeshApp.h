#ifndef __EXAMPLES_MOVE_MESH_APP__
#define __EXAMPLES_MOVE_MESH_APP__

#include <GLFWApp.h>

namespace examples
{
class MoveMeshApp : public GLFWApp
{
public:
    MoveMeshApp();

    void init(int argc, char** argv);

    void loop();

protected:
    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

    phyanim::Meshes _meshes;
    phyanim::Mesh* _mesh;
    uint32_t _meshId;

    bool _editing;
    bool _finished;
};

}  // namespace examples

#endif
