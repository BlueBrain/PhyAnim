#include "Triangle.h"

namespace phyanim{

  Triangle::Triangle( Node* n0_, Node* n1_, Node* n2_ )
    : _n0( n0_), _n1( n1_), _n2( n2_){
  }

  Triangle::~Triangle( void ){
  }

  Node* Triangle::node0( void ){
    return _n0;
  }

  void Triangle::node0( Node* node_ ){
    _n0 = node_;
  }

  Node* Triangle::node1( void ){
    return _n1;
  }

  void Triangle::node1( Node* node_ ){
    _n1 = node_;
  }

  Node* Triangle::node2( void ){
    return _n2;
  }

  void Triangle::node2( Node* node_ ){
    _n2 = node_;
  }

}
