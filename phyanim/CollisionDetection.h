#ifndef __PHYANIM_COLLISIONDETECTION__
#define __PHYANIM_COLLISIONDETECTION__

#include <Mesh.h>

namespace phyanim {

class CollisionDetection {

  public:

    CollisionDetection(double stiffness_=1000.0);

    virtual ~CollisionDetection(void);

    void addMesh(Mesh* mesh_);

    void clear(void);

    bool update(void);

    void checkLimitsCollision(void);

    Vec3 lowerLimit;

    Vec3 upperLimit;

    double stiffness;

  protected:

    bool _checkMeshesCollision(Mesh* m0_, Mesh* m1_);
    
    bool _checkTrianglesCollision(Triangle* t0_, Triangle* t1_);
    
    void _checkAndSetForce(Node* node_, Vec3 normal_, double dist_ );

    Meshes _meshes;


};

}

#endif
