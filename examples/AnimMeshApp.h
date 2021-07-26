#ifndef __EXAMPLES_ANIM_MESH_APP__
#define __EXAMPLES_ANIM_MESH_APP__

#include <AnimSystem.h>
#include <CollisionDetection.h>
#include <GLFWApp.h>

namespace examples
{
class AnimMeshApp : public GLFWApp
{
public:
    AnimMeshApp();

    void init(int argc, char** argv);

    void loop();

protected:
    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

private:
    enum SimSystem
    {
        exmass,
        immass,
        exfem,
        imfem
    };

    phyanim::AnimSystem* _animSys;

    phyanim::CollisionDetection* _collisionSys;

    phyanim::Meshes _meshes;

    bool _anim;
};

}  // namespace examples

#endif
