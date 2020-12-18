#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include <CollisionDetection.h>

namespace phyanim {

class AnimSystem {

  public:

    AnimSystem(CollisionDetection* collDetector_ = nullptr);

    virtual ~AnimSystem(void);

    bool step(double dt_);

    void addMesh(Mesh* mesh_);

    void clear(void);

    bool gravity;

    bool collisions;
    
  protected:

    virtual void _step(double dt_)=0;

    Meshes _meshes;

    CollisionDetection* _collisionDetector;

};

}

#endif
