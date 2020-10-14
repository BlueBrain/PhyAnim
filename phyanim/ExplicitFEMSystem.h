#ifndef __PHYANIM_EXPLICITFEMSYSTEM__
#define __PHYANIM_EXPLICITFEMSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class ExplicitFEMSystem : public AnimSystem {

  public:

    ExplicitFEMSystem(CollisionDetection* collDetector_ = nullptr);

    virtual ~ExplicitFEMSystem(void);

    void step(float dt_);

  private:

    void _polar(const Mat3& f_, Mat3& q_) const;    
    
};

}

#endif
