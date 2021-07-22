#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include <Mesh.h>

namespace phyanim
{
class AnimSystem
{
public:
    AnimSystem(void);

    virtual ~AnimSystem(void);

    virtual void step(float dt_) = 0;

    void addMesh(Mesh* mesh_);

    void gravity(bool gravity_);

    bool gravity(void);

    void floorCollsision(bool floorCollision_);

    bool floorCollision(void);

protected:
    bool _gravity;

    bool _floor;

    Meshes _meshes;
};

}  // namespace phyanim

#endif
