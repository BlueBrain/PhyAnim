#ifndef __PHYANIM_DRAWABLEMESH__
#define __PHYANIM_DRAWABLEMESH__

#include "Mesh.h"

namespace phyanim
{
class DrawableMesh : public Mesh
{
public:
    DrawableMesh(double stiffness_ = 1000.0,
                 double density_ = 10.0,
                 double damping_ = 1.0,
                 double poissonRatio_ = 0.499);

    virtual ~DrawableMesh(void);

    void render();

    void renderSurface();

    void upload(Vec3 color = Vec3(0.4, 0.4, 0.8));

    void uploadPositions();

    void uploadColors(Vec3 color = Vec3(0.4, 0.4, 0.8));

    void uploadColors(std::vector<double> colors);

private:
    std::vector<unsigned int> _vao;
    unsigned int _posVbo;
    unsigned int _colorVbo;
    size_t _indicesSize;
    size_t _surfaceIndicesSize;
};

}  // namespace phyanim

#endif
