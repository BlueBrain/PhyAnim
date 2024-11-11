#ifndef __EXAMPLES_ICOSPHERE__
#define __EXAMPLES_ICOSPHERE__

#include <phyanim/Phyanim.h>

namespace examples
{
class Icosphere : public phyanim::geometry::Mesh
{
public:
    Icosphere(phyanim::geometry::Vec3 pos = phyanim::geometry::Vec3(),
              float radius = 1.0f);

    virtual ~Icosphere(){};
};

}  // namespace examples
#endif
