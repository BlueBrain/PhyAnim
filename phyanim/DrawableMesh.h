#ifndef __PHYANIM_DRAWABLEMESH__
#define __PHYANIM_DRAWABLEMESH__

#include <Mesh.h>

namespace phyanim {

class DrawableMesh: public Mesh {

  public:

    DrawableMesh(double stiffness_ = 1000.0, double density_ = 10.0,
                 double damping_ = 1.0, double poissonRatio_ = 0.499);

    virtual ~DrawableMesh(void);

    void render(void);

    void renderSurface(void);

    void load(void);

    void loadNodes(void);

  private:

    unsigned int _vao[2];
    unsigned int _posVbo;
    size_t _indicesSize;
    size_t _surfaceIndicesSize;

};

}

#endif 
