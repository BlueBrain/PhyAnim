#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <Camera.h>
#include <RenderProgram.h>

#include <chrono>
#include <mutex>

namespace examples
{
class Scene
{
public:
    Scene();

    ~Scene();

    void addMesh(phyanim::DrawableMesh* mesh);

    void render();

    uint32_t picking(uint32_t x, uint32_t y);

    void cameraRatio(uint32_t width, uint32_t height);

    void cameraPosition(phyanim::Vec3 position);

    void displaceCamera(phyanim::Vec3 displace);

    void rotateCamera(double pitch, double yaw);

    void changeRenderMode();

    phyanim::Mat3 cameraRotation() const;

private:
    float* _idToColor4f(uint32_t id);

public:
    bool showFPS;

    std::vector<phyanim::DrawableMesh*> meshes;

private:
    typedef enum RenderMode
    {
        SOLID = 0,
        WIREFRAME
    } RenderMode;

    Camera* _camera;

    RenderMode _renderMode;
    bool _renderModeChanged;

    std::mutex _meshesMutex;

    RenderProgram* _program;
    RenderProgram* _pickingProgram;

    uint32_t _width;
    uint32_t _height;

    uint64_t _framesCount;

    std::chrono::time_point<std::chrono::steady_clock> _previousTime;
};

}  // namespace examples

#endif
