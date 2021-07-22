#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include <CollisionDetection.h>

namespace phyanim
{
class AnimSystem
{
public:
    AnimSystem(double dt);

    virtual ~AnimSystem(void);

    void step(void);

    virtual void addMesh(Mesh* mesh);

    void addMeshes(Meshes meshes);

    void clear(void);

protected:
    virtual void _step(void) = 0;

public:
    bool gravity;

protected:
    Meshes _meshes;

    double _dt;
};

}  // namespace phyanim

#endif
