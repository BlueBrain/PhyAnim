#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include "CollisionDetection.h"

namespace phyanim
{
namespace anim
{
#define THRESHOLD 0.01f

class AnimSystem
{
public:
    AnimSystem(float dt);

    virtual ~AnimSystem(void);

    void step(geometry::Mesh* mesh);

    void step(geometry::Meshes meshes);

    virtual void preprocessMesh(geometry::Mesh* mesh){};

    void preprocessMesh(geometry::Meshes meshes);

protected:
    virtual void _step(geometry::Mesh* mesh) = 0;

    void _addGravity(geometry::Nodes& nodes);

    void _update(geometry::Nodes& nodes);

    void _updateIfCollide(geometry::Nodes& nodes);

public:
    bool gravity;
    bool inertia;

protected:
    geometry::Meshes _meshes;

    float _dt;
};

}  // namespace anim
}  // namespace phyanim

#endif
