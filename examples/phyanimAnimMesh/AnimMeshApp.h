#ifndef __EXAMPLES_ANIM_MESH_APP__
#define __EXAMPLES_ANIM_MESH_APP__

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

    phyanim::Meshes _meshes;

    double _collisionStiffness;

    bool _anim;

    phyanim::Vec3 _pauseColor;
    phyanim::Vec3 _animColor;
};

}  // namespace examples

#endif
