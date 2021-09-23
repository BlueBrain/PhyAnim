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

    virtual ~DrawableMesh();

    void load(const std::string& file_);

    void load(const std::string& nodeFile_, const std::string& eleFile_);

    void render();

    void upload();

    void updateColor(phyanim::Vec3 color = phyanim::Vec3(0.4, 0.4, 0.8));

    void updateColors(phyanim::Vec3 staticColor = phyanim::Vec3(0.4, 0.4, 0.8),
                      phyanim::Vec3 dynamicColor = phyanim::Vec3(0.8, 0.4, 0.4),
                      phyanim::Vec3 collideColor = phyanim::Vec3(1.0, .0, .0));

    bool updatedPositions;
    bool updatedColors;

private:
    void _uploadPositions();
    void _uploadColors();

    unsigned int _vao;
    unsigned int _posVbo;
    unsigned int _colorVbo;
    size_t _indicesSize;
};

}  // namespace phyanim

#endif
