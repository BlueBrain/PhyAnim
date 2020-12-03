#ifndef __PHYANIM_MESH__
#define __PHYANIM_MESH__

#include <Tetrahedron.h>
#include <AxisAlignedBoundingBox.h>

namespace phyanim {

class Mesh;

typedef std::vector<Mesh*> Meshes;

class Mesh {

  public:

    Mesh(double stiffness_ = 1000.0, double density_ = 1.0,
         double damping_ = 1.0, double poissonRatio_ = 0.499);

    virtual ~Mesh(void);

    Nodes nodes;

    Edges edges;

    Tetrahedra tetrahedra;

    Triangles surfaceTriangles;

    Triangles triangles;

    double initVolume;
    
    double volume(void);

    double stiffness;

    double density;

    double damping;

    double poissonRatio;

    AxisAlignedBoundingBox* aabb;

    void trianglesToEdges(void);
    
    void tetsToNodes(void);

    void tetsToEdges(void);
    
    void tetsToTriangles(void);

    void nodesToInitPos(void);

};

}

#endif 
