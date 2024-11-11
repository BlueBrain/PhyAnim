#ifndef __EXAMPLES_RENDERPROGRAM__
#define __EXAMPLES_RENDERPROGRAM__

#include <string>

namespace phyanim
{
namespace graphics
{
class RenderProgram
{
public:
    RenderProgram(const std::string& vShaderSource,
                  const std::string& gShaderSource,
                  const std::string& fShaderSource);

    virtual ~RenderProgram(void);

    void use(void);

private:
    void _createProgram(const std::string& vShaderSource,
                        const std::string& gShaderSource,
                        const std::string& fShaderSource);

    void _deleteProgram(void);

    uint64_t _compileShader(const std::string& source, int type);

public:
    int64_t projviewmodelIndex;
    int64_t viewmodelIndex;
    int64_t pickingColor;

    uint64_t id;

private:
    uint64_t _vshader;
    uint64_t _gshader;
    uint64_t _fshader;
};

}  // namespace graphics
}  // namespace phyanim

#endif