#include "Node.h"

namespace phyanim
{
Node::Node(Vec3 position,
           unsigned int id,
           Vec3 velocity,
           Vec3 force,
           double mass,
           bool surface,
           bool fix)
    : initPosition(position)
    , position(position)
    , normal(Vec3::UnitZ())
    , id(id)
    , velocity(velocity)
    , force(force)
    , color(0.4, 0.4, 0.8)
    , mass(mass)
    , surface(surface)
    , fix(fix)
    , anim(false)
    , collide(false)
{
}

Node::~Node() {}

bool Node::operator==(const Node& other_) const
{
    return (position == other_.position) && (id == other_.id);
}

bool Node::operator!=(const Node& other_) const { return !(*this == other_); }

}  // namespace phyanim
