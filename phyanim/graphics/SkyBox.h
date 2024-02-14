#ifndef __EXAMPLES_SKYBOX__
#define __EXAMPLES_SKYBOX__

#include "Camera.h"
#include "RenderProgram.h"
#include "Texture.h"

namespace phyanim
{
namespace graphics
{
class SkyBox
{
public:
    SkyBox(const std::string& file);

    void render(Camera* camera);

protected:
    RenderProgram* _program;
    Texture* _texture;

    uint32_t _uView;
    uint32_t _uProj;
    uint32_t _vao;

    uint32_t _numIndices;
};

}  // namespace graphics
}  // namespace phyanim

#endif