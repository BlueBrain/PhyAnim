#ifndef __PHYANIM_EDGE__
#define __PHYANIM_EDGE__

#include <Node.h>


namespace phyanim{

class Edge;

typedef std::vector<Edge*> Edges;

class Edge {

  public:

    Edge(Node* node0_, Node* node1_);

    virtual ~Edge(void);

    Node* node0(void);

    Node* node1(void);

    Vec3 direction(void);
    
    float length(void);

    float lengthOffset(void);
    
  private:

    Node* _n0;
    Node* _n1;

    float _initLength;
   
};

}

#endif
