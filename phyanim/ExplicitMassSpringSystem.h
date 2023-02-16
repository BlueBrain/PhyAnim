#ifndef __PHYANIM_EXPLICITMASSSPRINGSYSTEM__
#define __PHYANIM_EXPLICITMASSSPRINGSYSTEM__

#include <AnimSystem.h>

namespace phyanim
{
class ExplicitMassSpringSystem : public AnimSystem
{
public:
    ExplicitMassSpringSystem(double dt);

    virtual ~ExplicitMassSpringSystem(void);

    void step(Nodes& nodes,
              Edges& edges,
              AxisAlignedBoundingBox& limits,
              double ks,
              double kd);

protected:
    void _step(Mesh* mesh);
};

}  // namespace phyanim

#endif