#ifndef __PHYANIM_TETRAHEDRON__
#define __PHYANIM_TETRAHEDRON__

#include <Edge.h>
#include <Triangle.h>

namespace phyanim{

class Tetrahedron;

typedef std::vector<Tetrahedron*> Tetrahedra;

class Tetrahedron{

  public:

    Tetrahedron(Node* n0_, Node* n1_, Node* n2_, Node* n3_);
    
    virtual ~Tetrahedron(void);

    Node* node0;

    Node* node1;

    Node* node2;

    Node* node3;

    Mat3 basis;

    Mat3 invBasis;

    Mat3 k00;
    Mat3 k11;
    Mat3 k22;
    Mat3 k33;
    Mat3 k01;
    Mat3 k02;
    Mat3 k03;
    Mat3 k12;
    Mat3 k13;
    Mat3 k23;

    Vec3 normal0;

    Vec3 normal1;

    Vec3 normal2;

    Vec3 normal3;

    double initVolume;

    double volume(void) const;

    Edges edges(void);
      
    Triangles triangles(void);

  private:

    double _volume;
    

};

}

#endif
