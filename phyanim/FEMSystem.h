#ifndef __PHYANIM_FEMSYSTEM__
#define __PHYANIM_FEMSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class FEMSystem : public AnimSystem {

  public:

    FEMSystem(void);

    virtual ~FEMSystem(void);

    void step(float dt_);
    
};

}

#endif
