#ifndef __PHYANIM_TRIANGLE__
#define __PHYANIM_TRIANGLE__

#include "Edge.h"

namespace phyanim {

class Triangle;

typedef std::vector<Triangle*> Triangles;

class Triangle {

  public:

    Triangle(Node* n0_, Node* n1_, Node* n2_);

    virtual ~Triangle(void);

    Node* node0(void);

    void node0(Node* node_);

    Node* node1(void);

    void node1(Node* node_);

    Node* node2(void);

    void node2(Node* node_);

    Edges edges(void);
      

  private:

    Node* _n0;
    Node* _n1;
    Node* _n2;

};

}

#endif
