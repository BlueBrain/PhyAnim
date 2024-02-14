#ifndef __PHYANIM_EXPLICITMASSSPRINGSYSTEM__
#define __PHYANIM_EXPLICITMASSSPRINGSYSTEM__

#include "AnimSystem.h"

namespace phyanim
{
namespace anim
{
class ExplicitMassSpringSystem : public AnimSystem
{
public:
    ExplicitMassSpringSystem(float dt);

    virtual ~ExplicitMassSpringSystem(void);

    void step(geometry::Nodes& nodes,
              geometry::Edges& edges,
              geometry::AxisAlignedBoundingBox& limits,
              float ks,
              float kd);

protected:
    void _step(geometry::Mesh* mesh);
};

}  // namespace anim
}  // namespace phyanim

#endif