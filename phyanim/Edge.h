#ifndef __PHYANIM_EDGE__
#define __PHYANIM_EDGE__

#include <unordered_set>

#include <Node.h>


namespace phyanim{

class Edge;

typedef std::vector<Edge*> Edges;

struct EdgePointerHash { 
  public:
    size_t operator()(const Edge* edge_) const;
};


struct EdgePointerEqual { 
  public:
    bool operator()(const Edge* edge0_, const Edge* edge1_) const;
}; 

typedef std::unordered_set<Edge*, EdgePointerHash, EdgePointerEqual> UniqueEdges;

class Edge {

  public:

    Edge(Node* node0_, Node* node1_);

    virtual ~Edge(void);

    Node* node0(void);

    Node* node0(void) const;

    Node* node1(void);

    Node* node1(void) const;
    
    double restLength(void);

    bool operator==(const Edge& edge_) const;
    
  private:

    Node* _n0;
    Node* _n1;

    double _initLength;
   
};

}

#endif
