#ifndef __EXAMPLES_COLOR_PALETTE__
#define __EXAMPLES_COLOR_PALETTE__

#include <Phyanim.h>

namespace examples
{

typedef enum
{
    SOFT = 0,
    CONTRAST,
    SCALE
} PaletteType;

class ColorPalette
{
public:
    ColorPalette(PaletteType type = SOFT)
    {
        switch (type)
        {
        case SOFT:
            _addColor((uint8_t)0x9f, (uint8_t)0x9f, (uint8_t)0x9d);
            _addColor((uint8_t)0x8e, (uint8_t)0xb9, (uint8_t)0xd1);
            _addColor((uint8_t)0xea, (uint8_t)0x90, (uint8_t)0x90);
            _addColor((uint8_t)0xa2, (uint8_t)0xd2, (uint8_t)0x9e);
            _addColor((uint8_t)0xf9, (uint8_t)0xc1, (uint8_t)0x98);
            _addColor((uint8_t)0xb7, (uint8_t)0x97, (uint8_t)0xbd);
            _addColor((uint8_t)0xd6, (uint8_t)0xa1, (uint8_t)0x8c);
            _addColor((uint8_t)0xd9, (uint8_t)0xaa, (uint8_t)0xc6);
            break;

        case CONTRAST:
            _addColor((uint8_t)0x89, (uint8_t)0x31, (uint8_t)0xef);
            _addColor((uint8_t)0xf2, (uint8_t)0xca, (uint8_t)0x19);
            _addColor((uint8_t)0xff, (uint8_t)0x00, (uint8_t)0xbd);
            _addColor((uint8_t)0x00, (uint8_t)0x57, (uint8_t)0xe9);
            _addColor((uint8_t)0x87, (uint8_t)0xe9, (uint8_t)0x11);
            _addColor((uint8_t)0xe1, (uint8_t)0x18, (uint8_t)0x45);
            break;
        case SCALE:
            _addColor((uint8_t)0x00, (uint8_t)0x00, (uint8_t)0x00);
            _addColor((uint8_t)0x05, (uint8_t)0x0a, (uint8_t)0x30);
            _addColor((uint8_t)0x00, (uint8_t)0x0c, (uint8_t)0x66);
            _addColor((uint8_t)0x00, (uint8_t)0x00, (uint8_t)0xff);
            _addColor((uint8_t)0x7e, (uint8_t)0xc8, (uint8_t)0xe3);
            _addColor((uint8_t)0xff, (uint8_t)0xff, (uint8_t)0xff);
            break;
        }
    };

    phyanim::Vec3 color(uint32_t id)
    {
        if (_colors.size() <= 0) return phyanim::Vec3::Zero();

        return _colors[id % _colors.size()];
    };

    phyanim::Vec3 color(double f)
    {
        if (_colors.size() <= 0) return phyanim::Vec3::Zero();

        return _colors[(uint32_t)(f * (_colors.size() + 1)) % _colors.size()];
    };

    phyanim::Vec3 collisionColor() { return phyanim::Vec3(0.8f, .0f, .0f); };

protected:
    void _addColor(float r, float g, float b)
    {
        _colors.push_back(phyanim::Vec3(r, g, b));
    };

    void _addColor(uint8_t r, uint8_t g, uint8_t b)
    {
        _colors.push_back(phyanim::Vec3(r / 255.0f, g / 255.0f, b / 255.0f));
    };

    std::vector<phyanim::Vec3> _colors;
};
}  // namespace examples

#endif