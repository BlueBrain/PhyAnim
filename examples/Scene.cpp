#include "Scene.h"

#include <GL/glew.h>

#include <cstdlib>
#include <iostream>

namespace examples
{
const std::string vshaderSource(
    "#version 400\n"
    "in vec3 inPos;"
    "out vec3 position;"
    "uniform mat4 projViewModel;"
    "uniform mat4 viewModel;"
    "void main(void) {"
    "position = (viewModel*vec4(inPos, 1.0)).xyz;"
    "gl_Position = projViewModel * vec4(inPos, 1.0);}");

const std::string gshaderSource(
    "#version 400\n"
    "layout(triangles) in;"
    "layout(triangle_strip, max_vertices = 3) out;"
    "in vec3 position[3];"
    "out vec3 normal;"
    "out vec3 gposition;"
    "void main(void) {"
    "vec3 axis0 = normalize(position[1]-position[0]);"
    "vec3 axis1 = normalize(position[2]-position[0]);"
    "normal = cross(axis0, axis1);"
    "gposition = position[0];"
    "gl_Position = gl_in[0].gl_Position;"
    "EmitVertex();"
    "gposition = position[1];"
    "gl_Position = gl_in[1].gl_Position;"
    "EmitVertex();"
    "gposition = position[2];"
    "gl_Position = gl_in[2].gl_Position;"
    "EmitVertex();"
    "EndPrimitive();}");

const std::string fshaderSource(
    "#version 400\n"
    "in vec3 normal;"
    "in vec3 gposition;"
    "out vec4 oColor;"
    "void main(void){"
    "vec3 N = normalize( normal );"
    "vec3 L = normalize( -gposition );"
    "float diff = dot( N, L );"
    "diff = clamp( diff, 0.0, 1.0 );"
    "vec3 color = vec3( 0.5, 0.8, 0.2 );"
    "oColor = vec4( diff*color*0.8+color*0.2, 1.0 );}");

Scene::Scene() : showFPS(false), _renderMode(SOLID), _framesCount(0)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    _camera = new Camera();
    _program = new RenderProgram(vshaderSource, gshaderSource, fshaderSource);
    _previousTime = std::chrono::steady_clock::now();
}

Scene::~Scene()
{
    clear();
    delete _program;
}

void Scene::render()
{
    ++_framesCount;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _program->use();
    Eigen::Matrix4f projView = _camera->projectionViewMatrix().cast<float>();
    Eigen::Matrix4f view = _camera->viewMatrix().cast<float>();
    glUniformMatrix4fv(_program->projviewmodelIndex, 1, GL_FALSE,
                       projView.data());
    glUniformMatrix4fv(_program->viewmodelIndex, 1, GL_FALSE, view.data());
    for (auto mesh : _meshes)
    {
        mesh->renderSurface();
    }
    auto currentTime = std::chrono::steady_clock::now();
    double elapsed_seconds =
        (std::chrono::duration<double>(currentTime - _previousTime)).count();
    if (elapsed_seconds >= 1.0)
    {
        if (showFPS)
            std::cout << "FPS: " << _framesCount / elapsed_seconds << std::endl;
        _previousTime = currentTime;
        _framesCount = 0;
    }
}

void Scene::addMesh(phyanim::DrawableMesh* mesh) { _meshes.push_back(mesh); }

void Scene::clear() { _meshes.clear(); }

void Scene::cameraRatio(uint32_t width, uint32_t height)
{
    _camera->ratio((double)width / height);
    glViewport(0, 0, width, height);
}

void Scene::cameraPosition(phyanim::Vec3 position)
{
    _camera->position(position);
}

void Scene::displaceCamera(phyanim::Vec3 displace)
{
    phyanim::Vec3 rotDis = _camera->rotation() * displace;
    _camera->position(_camera->position() + rotDis);
}

void Scene::rotateCamera(double pitch, double yaw)
{
    double cosX = cos(pitch);
    double sinX = sin(pitch);
    double cosY = cos(yaw);
    double sinY = sin(yaw);
    phyanim::Mat3 rotX;
    rotX << 1.0, 0.0, 0.0, 0.0, cosX, -sinX, 0.0, sinX, cosX;
    phyanim::Mat3 rotY;
    rotY << cosY, 0.0, sinY, 0.0, 1.0, 0.0, -sinY, 0.0, cosY;
    phyanim::Mat3 rot = rotX * rotY;
    _camera->rotation(_camera->rotation() * rot);
}

void Scene::changeRenderMode()
{
    switch (_renderMode)
    {
    case SOLID:
        _renderMode = WIREFRAME;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glCullFace(GL_FRONT_AND_BACK);
        break;
    case WIREFRAME:
        _renderMode = SOLID;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glCullFace(GL_FRONT);
        break;
    }
}

}  // namespace examples
