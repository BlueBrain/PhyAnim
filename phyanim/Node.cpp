#include "Node.h"

namespace phyanim{

Node::Node(Vec3 position_, unsigned int id_, Vec3 velocity_, Vec3 force_,
           double mass_, bool surface_)                                                          
    : initPosition(position_), position(position_), id(id_)
    , initVelocity(velocity_), velocity(velocity_), force(force_)
    , mass(mass_), surface(surface_) {

}

Node::~Node() {

}

}
