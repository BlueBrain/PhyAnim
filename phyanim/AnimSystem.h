#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include <CollisionDetection.h>

namespace phyanim {

class AnimSystem {

  public:

    AnimSystem(double dt, CollisionDetection* collDetector_ = nullptr);

    virtual ~AnimSystem(void);

    bool step(void);

    virtual void addMesh(Mesh* mesh_);

    void clear(void);

    bool gravity;

    bool collisions;
    

  protected:

    virtual void _step(void)=0;

    Meshes _meshes;

    double _dt;
    
    CollisionDetection* _collisionDetector;

};

}

#endif
