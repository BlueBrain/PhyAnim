#ifndef __PHYANIM_NODE__
#define __PHYANIM_NODE__

#include <Eigen/Dense>
#include <vector>

namespace phyanim{

typedef Eigen::Vector4f Vec4;
typedef Eigen::Vector3f Vec3;
typedef Eigen::Vector2f Vec2;

  
typedef Eigen::Matrix4f Mat4;
typedef Eigen::Matrix3f Mat3;

class Node;

typedef std::vector<Node*> Nodes;

class Node
{

  public:

    Node(Vec3 position_ = Vec3(), unsigned int id_ = 0, bool surface_ = false,
         Vec3 v0_ = Vec3(.0f, .0f, .0f), Vec3 a0_ = Vec3(.0f, .0f, .0f));

    virtual ~Node(void);

    Vec3 position(void) const;

    void position(Vec3 position_);

    Vec3 initPosition(void) const;

    void initPosition(Vec3 position_);

    Vec3 v0(void) const;

    void v0(Vec3 v0_);

    Vec3 a0(void) const;

    void a0(Vec3 a0_);
    
    unsigned int id(void) const;

    void id(unsigned int id_);

    bool surface(void);

    void surface(bool surface_);

  private:

    Vec3 _initPosition;
    Vec3 _position;
    unsigned int _id;
    bool _surface;

    Vec3 _v0;
    Vec3 _a0;

};

}

#endif
