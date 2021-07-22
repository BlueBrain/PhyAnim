#ifndef __PHYANIM_DRAWABLEMESH__
#define __PHYANIM_DRAWABLEMESH__

#include <Mesh.h>

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

    void render(void);

    void renderSurface(void);

    void upload(void);

    void uploadNodes(void);

private:
    std::vector<unsigned int> _vao;
    unsigned int _posVbo;
    size_t _indicesSize;
    size_t _surfaceIndicesSize;
};

}  // namespace phyanim

#endif
