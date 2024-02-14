#include "Icosphere.h"

namespace examples
{
Icosphere::Icosphere(phyanim::geometry::Vec3 pos, float radius)
{
    load(PATH_TO_ICO);

    for (auto node : nodes)
    {
        node->position = node->position * radius + pos;
        node->initPosition = node->position;
    }
}

}  // namespace examples
