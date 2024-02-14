#ifndef __PHYANIM_NODE__
#define __PHYANIM_NODE__

#include <vector>

#include "Math.h"

namespace phyanim
{
namespace geometry
{
class Node;

typedef Node* NodePtr;

typedef std::vector<NodePtr> Nodes;

class Node
{
public:
    Node(Vec3 position_,
         unsigned int id_ = 0,
         float radius = 1.0,
         Vec3 velocity_ = Vec3(),
         Vec3 force_ = Vec3(),
         float mass_ = 1.0,
         bool surface_ = false,
         bool fixed_ = false);

    virtual ~Node(void);

    bool operator==(const Node& other_) const;

    bool operator!=(const Node& other_) const;

    Vec3 initPosition;

    Vec3 position;

    float radius;

    Vec3 normal;

    Vec3 velocity;

    Vec3 force;

    float mass;

    unsigned int id;

    bool surface;

    bool fix;

    bool isSoma;

    bool anim;

    bool collide;
};

void clearForce(Nodes& nodes);

void clearCollision(Nodes& nodes);

void clearVelocityIfNoColl(Nodes& nodes);

}  // namespace geometry
}  // namespace phyanim

#endif
