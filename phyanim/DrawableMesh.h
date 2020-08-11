#ifndef __PHYANIM_DRAWABLEMESH__
#define __PHYANIM_DRAWABLEMESH__

#include <Mesh.h>

namespace phyanim {

class DrawableMesh: public Mesh {

  public:

    DrawableMesh(void);

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
