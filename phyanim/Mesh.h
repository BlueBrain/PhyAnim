#ifndef __PHYANIM_MESH__
#define __PHYANIM_MESH__

#include <Tetrahedron.h>

namespace phyanim {

class Mesh;

typedef std::vector<Mesh*> Meshes;

class Mesh {

  public:

    Mesh(float stiffness_ = 10.0f);

    virtual ~Mesh(void);

    Nodes& nodes(void);

    Edges& edges(void);

    Tetrahedra& tetrahedra(void);

    Triangles& surfaceTriangles(void);

    Triangles& triangles(void);

    float stiffness(void);

    void stiffness(float stiffness_);

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

    float _stiffness;
};

}

#endif 
