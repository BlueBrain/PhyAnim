#include "Node.h"

namespace phyanim
{
Node::Node(Vec3 position,
           unsigned int id,
           double radius,
           Vec3 velocity,
           Vec3 force,
           double mass,
           bool surface,
           bool fix)
    : initPosition(position)
    , position(position)
    , radius(radius)
    , normal(Vec3::UnitZ())
    , id(id)
    , velocity(velocity)
    , force(force)
    , mass(mass)
    , surface(surface)
    , fix(fix)
    , isSoma(false)
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

void clearForce(Nodes& nodes)
{
    uint32_t size = nodes.size();
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i) nodes[i]->force = Vec3::Zero();
}

void clearCollision(Nodes& nodes)
{
    uint32_t size = nodes.size();
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i) nodes[i]->collide = false;
}

void clearVelocityIfNoColl(Nodes& nodes)
{
    uint32_t size = nodes.size();
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
        if (!nodes[i]->collide) nodes[i]->velocity = Vec3::Zero();
}

}  // namespace phyanim
