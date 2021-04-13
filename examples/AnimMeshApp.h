#ifndef __EXAMPLES_ANIM_MESH_APP__
#define __EXAMPLES_ANIM_MESH_APP__

#include <GLFWApp.h>
#include <ImplicitFEMSystem.h>

namespace examples
{

class AnimMeshApp: public GLFWApp
{

  public:

    AnimMeshApp();

    void init(int argc, char** argv);

    void loop();

  protected:

    void _keyCallback(GLFWwindow* window, int key, int scancode, int action,
                      int mods);


  private:

    phyanim::AnimSystem* _animSys;

    phyanim::CollisionDetection* _collisionSys;
    
    phyanim::DrawableMesh* _mesh;

    bool _anim;
};

}

#endif 
