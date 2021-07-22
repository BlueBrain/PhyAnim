#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <chrono>

#include <Camera.h>
#include <DrawableMesh.h>
#include <RenderProgram.h>


namespace examples{

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

}

#endif
