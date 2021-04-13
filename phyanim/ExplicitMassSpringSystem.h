#ifndef __PHYANIM_EXPLICITMASSSPRINGSYSTEM__
#define __PHYANIM_EXPLICITMASSSPRINGSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class ExplicitMassSpringSystem : public AnimSystem {

  public:
    
    ExplicitMassSpringSystem(double dt);

    virtual ~ExplicitMassSpringSystem(void);

  protected:

    void _step(void);
};

}

#endif
