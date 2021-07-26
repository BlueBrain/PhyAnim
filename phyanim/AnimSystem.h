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

    void step(Mesh* mesh);

    void step(Meshes meshes);

    virtual void preprocessMesh(Mesh* mesh){};

    void preprocessMesh(Meshes meshes);

protected:
    virtual void _step(Mesh* mesh) = 0;

public:
    bool gravity;

protected:
    Meshes _meshes;

    double _dt;
};

}  // namespace phyanim

#endif
