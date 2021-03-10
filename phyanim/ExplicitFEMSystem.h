#ifndef __PHYANIM_EXPLICITFEMSYSTEM__
#define __PHYANIM_EXPLICITFEMSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class ExplicitFEMSystem : public AnimSystem {

  public:

    ExplicitFEMSystem(double dt, CollisionDetection* collDetector_ = nullptr);

    virtual ~ExplicitFEMSystem(void);

  private:
    
    void _step(void);

    void _polar(const Mat3& f_, Mat3& q_) const;    
    
};

}

#endif
