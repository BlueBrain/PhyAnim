#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include <Mesh.h>

namespace phyanim {

class AnimSystem {

  public:

    AnimSystem(void);

    virtual ~AnimSystem(void);

    virtual void step(float dt_) = 0;

    virtual void addMesh(Mesh* mesh_);

    void gravity(bool gravity_);

    bool gravity(void);

    void floorCollision(bool floorCollision_);

    bool floorCollision(void);

  protected:

    bool _gravity;

    bool _floor;
    float _floorHeight;

    Meshes _meshes;

};

}

#endif
