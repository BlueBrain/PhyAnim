#ifndef __EXAMPLES_SKYBOX__
#define __EXAMPLES_SKYBOX__

#include <Phyanim.h>
#include "Texture.h"
#include "RenderProgram.h"
#include "Camera.h"

namespace examples
{
class SkyBox
{
public:
    SkyBox();

    void render(Camera* camera);


protected:

    RenderProgram* _program;
    Texture* _texture;

    uint32_t _uProjView;
    uint32_t _vao;

};
}

#endif