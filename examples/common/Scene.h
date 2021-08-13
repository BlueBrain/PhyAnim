#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <Camera.h>
#include <RenderProgram.h>

#include <chrono>

namespace examples
{
class Scene
{
public:
    Scene();

    ~Scene();

    void render();

    void addMesh(phyanim::DrawableMesh* mesh);

    void clear();

    void cameraRatio(uint32_t width, uint32_t height);

    void cameraPosition(phyanim::Vec3 position);

    void displaceCamera(phyanim::Vec3 displace);

    void rotateCamera(double pitch, double yaw);

    void changeRenderMode();

    void updateColors(phyanim::Vec3 staticColor = phyanim::Vec3(0.4, 0.4, 0.8),
                      phyanim::Vec3 dynamicColor = phyanim::Vec3(0.8, 0.4, 0.4),
                      phyanim::Vec3 collideColor = phyanim::Vec3(1.0,
                                                                 0.0,
                                                                 0.0));

public:
    bool showFPS;

private:
    typedef enum RenderMode
    {
        SOLID = 0,
        WIREFRAME
    } RenderMode;

    Camera* _camera;

    std::vector<phyanim::DrawableMesh*> _meshes;

    RenderMode _renderMode;

    RenderProgram* _program;

    uint64_t _framesCount;

    std::chrono::time_point<std::chrono::steady_clock> _previousTime;
};

}  // namespace examples

#endif
