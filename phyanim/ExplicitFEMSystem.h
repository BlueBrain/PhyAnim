#ifndef __PHYANIM_EXPLICITFEMSYSTEM__
#define __PHYANIM_EXPLICITFEMSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class ExplicitFEMSystem : public AnimSystem {

  public:

    ExplicitFEMSystem(CollisionDetection* collDetector_ = nullptr);

    virtual ~ExplicitFEMSystem(void);

  private:
    
    void _step(double dt_);

    void _polar(const Mat3& f_, Mat3& q_) const;    
    
};

}

#endif
