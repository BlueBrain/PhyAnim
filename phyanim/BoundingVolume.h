#ifndef __PHYANIM_BOUNDINGVOLUME__
#define __PHYANIM_BOUNDINGVOLUME__

#include <Node.h>

namespace phyanim {

class BoundingVolume {
    
  public:
    
    BoundingVolume(void);
    
    virtual ~BoundingVolume(void);
    
    virtual void update(Nodes& nodes_) = 0;
    
    virtual bool checkCollision(BoundingVolume* other_) = 0;

    virtual bool checkLimitsCollision(const Vec3& lowerLimit_, const Vec3& upperLimit_) = 0;

    
};

}


#endif
