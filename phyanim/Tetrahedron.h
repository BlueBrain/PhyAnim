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
    
    Node* node0(void);

    void node0(Node* node_);

    Node* node1(void);

    void node1(Node* node_);

    Node* node2(void);

    void node2(Node* node_);

    Node* node3(void);

    void node3(Node* node_);

    Mat3 basis(void) const;

    Vec3 normal0(void) const;

    Vec3 normal1(void) const;

    Vec3 normal2(void) const;

    Vec3 normal3(void) const;

    double volume(void) const;

    Edges edges(void);
      
    Triangles triangles(void);

  private:

    Node* _n0;
    Node* _n1;
    Node* _n2;
    Node* _n3;

    Mat3 _basis;
    Vec3 _normal0;
    Vec3 _normal1;
    Vec3 _normal2;
    Vec3 _normal3;
    double _volume;
    

};

}

#endif
