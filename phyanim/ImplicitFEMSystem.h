#ifndef __PHYANIM_IMPLICITFEMSYSTEM__
#define __PHYANIM_IMPLICITFEMSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class ImplicitFEMSystem : public AnimSystem {

  public:

    ImplicitFEMSystem(CollisionDetection* collDetector_ = nullptr);

    virtual ~ImplicitFEMSystem(void);

    void step(float dt_);

  private:

    void _polar(const Mat3& f_, Mat3& q_) const;    
    
};

}

#endif
