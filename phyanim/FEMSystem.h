#ifndef __PHYANIM_FEMSYSTEM__
#define __PHYANIM_FEMSYSTEM__

#include <AnimSystem.h>

namespace phyanim {

class FEMSystem : public AnimSystem {

  public:

    FEMSystem(void);

    virtual ~FEMSystem(void);

    void step(float dt_);

    void addMesh(Mesh* mesh_);

  private:

    void _polar(const Mat3& f_, Mat3& q_) const;    
    
};

}

#endif
