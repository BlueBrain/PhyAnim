#ifndef __PHYANIM_MASSSPRINGSYSTEM__
#define __PHYANIM_MASSSPRINGSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class MassSpringSystem : public AnimSystem {

  public:
    
    MassSpringSystem(void);

    virtual ~MassSpringSystem(void);

    void step(float dt_);

    void addMesh(Mesh* mesh_);

};

}

#endif
