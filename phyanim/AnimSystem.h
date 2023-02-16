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

    void clearForce(Nodes& nodes);

    void clearCollision(Nodes& nodes);
    void clearCollision(Edges& edges);

protected:
    virtual void _step(Mesh* mesh) = 0;

    void _addGravity(Nodes& nodes);

    void _update(Nodes& nodes);

    void _updateIfCollide(Nodes& nodes);

public:
    bool gravity;
    bool inertia;

protected:
    Meshes _meshes;

    double _dt;
};

}  // namespace phyanim

#endif
