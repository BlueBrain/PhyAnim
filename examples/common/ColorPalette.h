#ifndef __EXAMPLES_COLOR_PALETTE__
#define __EXAMPLES_COLOR_PALETTE__

#include <Phyanim.h>

namespace examples
{

class ColorPalette
{
public:
    ColorPalette()
    {
        _addColor((uint8_t)0x9f, (uint8_t)0x9f, (uint8_t)0x9d);
        _addColor((uint8_t)0xea, (uint8_t)0x90, (uint8_t)0x90);
        _addColor((uint8_t)0xa2, (uint8_t)0xd2, (uint8_t)0x9e);
        _addColor((uint8_t)0x8e, (uint8_t)0xb9, (uint8_t)0xd1);
        _addColor((uint8_t)0xf9, (uint8_t)0xc1, (uint8_t)0x98);
        _addColor((uint8_t)0xb7, (uint8_t)0x97, (uint8_t)0xbd);
        _addColor((uint8_t)0xd6, (uint8_t)0xa1, (uint8_t)0x8c);
        _addColor((uint8_t)0xd9, (uint8_t)0xaa, (uint8_t)0xc6);
    };

    phyanim::Vec3 color(uint32_t id)
    {
        if (_colors.size() <= 0)
            return phyanim::Vec3::Zero();

        return _colors[id%_colors.size()];
    };

    phyanim::Vec3 collisionColor() { return phyanim::Vec3(0.5f, .0f, .0f); };

protected:

    void _addColor(float r, float g, float b)
    {
        _colors.push_back(phyanim::Vec3(r, g, b));
    };

    void _addColor(uint8_t r, uint8_t g, uint8_t b)
    {
        _colors.push_back(phyanim::Vec3(r/255.0f, g/255.0f, b/255.0f));
    };

    std::vector<phyanim::Vec3> _colors;

};
}


#endif