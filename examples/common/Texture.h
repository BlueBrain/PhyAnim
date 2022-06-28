#ifndef __EXAMPLES_TEXTURE__
#define __EXAMPLES_TEXTURE__

#include <Phyanim.h>

namespace examples
{
class Texture
{
public:
    Texture(const std::string& fileName = "",
            uint32_t width = 600,
            uint32_t height = 600);

    void loadPng(const std::string& fileName);

    uint32_t id;

private:
    uint32_t _width;
    uint32_t _height;

};
}  // namespace xm

#endif