#ifndef __PHYANIM_NODE__
#define __PHYANIM_NODE__

#include <Eigen/Dense>
#include <vector>

namespace phyanim{

typedef Eigen::Vector4d Vec4;
typedef Eigen::Vector3d Vec3;
typedef Eigen::Vector2d Vec2;

  
typedef Eigen::Matrix4d Mat4;
typedef Eigen::Matrix3d Mat3;

class Node;

typedef std::vector<Node*> Nodes;

class Node
{

  public:

    Node(Vec3 position_, unsigned int id_ = 0,
         Vec3 velocity_ = Vec3::Zero(), Vec3 force_ = Vec3::Zero(),
         double mass_ = 1.0, bool surface_ = false);

    virtual ~Node(void);

    bool operator ==(const Node& other_) const;
    
    bool operator !=(const Node& other_) const;

    Vec3 initPosition;

    Vec3 position;

    Vec3 initVelocity;

    Vec3 velocity;

    Vec3 force;

    double mass;

    unsigned int id;

    bool surface;

};

}

#endif
