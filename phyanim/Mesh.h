#ifndef __PHYANIM_MESH__
#define __PHYANIM_MESH__

#include <Tetrahedron.h>

namespace phyanim {

class Mesh;

typedef std::vector<Mesh*> Meshes;

class Mesh {

  public:

    Mesh(double stiffness_ = 1000.0, double density_ = 1.0,
         double damping_ = 1.0, double poissonRatio_ = 0.499);

    virtual ~Mesh(void);

    Nodes& nodes(void);

    Edges& edges(void);

    Tetrahedra& tetrahedra(void);

    Triangles& surfaceTriangles(void);

    Triangles& triangles(void);

    double stiffness(void);

    void stiffness(double stiffness_);

    double density(void);

    void density(double density_);

    double damping(void);

    void damping(double damping_);

    double poissonRatio(void);

    void poissonRatio(double poissonRatio_);
    
    void trianglesToEdges(void);
    
    void tetsToNodes(void);

    void tetsToEdges(void);
    
    void tetsToTriangles(void);

    void nodesToInitPos(void);

  protected:

    Nodes _nodes;
    Edges _edges;
    Tetrahedra _tetrahedra;
    Triangles _surfaceTriangles;
    Triangles _triangles;

    double _stiffness;
    double _damping;
    double _poissonRatio;
    double _density;
};

}

#endif 
