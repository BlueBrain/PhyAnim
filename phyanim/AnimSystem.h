#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include <CollisionDetection.h>

namespace phyanim {

class AnimSystem {

  public:

    AnimSystem(CollisionDetection* collDetector_ = nullptr);

    virtual ~AnimSystem(void);

    virtual void step(float dt_);

    virtual void addMesh(Mesh* mesh_);

    void clear(void);

    bool gravity;

    bool limitsCollision;
    
  protected:

    void _checkLimitsCollision(void);

    Meshes _meshes;

    CollisionDetection* _collisionDetector;

};

}

#endif
