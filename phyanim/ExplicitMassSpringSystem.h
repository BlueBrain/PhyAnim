#ifndef __PHYANIM_EXPLICITMASSSPRINGSYSTEM__
#define __PHYANIM_EXPLICITMASSSPRINGSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class ExplicitMassSpringSystem : public AnimSystem {

  public:
    
    ExplicitMassSpringSystem(CollisionDetection* collDetector_ = nullptr);

    virtual ~ExplicitMassSpringSystem(void);

    void step(float dt_);

};

}

#endif
