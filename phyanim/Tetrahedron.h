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

    Edges edges(void);
      
    Triangles triangles(void);

  private:

    Node* _n0;
    Node* _n1;
    Node* _n2;
    Node* _n3;
};

}

#endif
