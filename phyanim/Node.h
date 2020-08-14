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

    Vec3 position(void) const;

    void position(Vec3 position_);

    Vec3 initPosition(void) const;

    Vec3 velocity(void) const;

    void velocity(Vec3 velocity_);

    Vec3 force(void) const;

    void force(Vec3 acceleration_);

    double mass(void) const;

    void mass(double mass_);
    
    unsigned int id(void) const;

    void id(unsigned int id_);

    bool surface(void);

    void surface(bool surface_);

  private:

    Vec3 _initPosition;
    Vec3 _position;
    Vec3 _velocity;
    Vec3 _force;

    double _mass;

    unsigned int _id;
    bool _surface;

};

}

#endif
