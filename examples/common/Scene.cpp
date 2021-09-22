#include "Scene.h"

#include <GL/glew.h>

#include <cstdlib>
#include <iostream>

namespace examples
{
const std::string vPickingSource(
    "#version 400\n"
    "in vec3 inPos;"
    "uniform mat4 projViewModel;"
    "void main(void) {"
    "gl_Position = projViewModel * vec4(inPos, 1.0);}");

const std::string fPickingSource(
    "#version 400\n"
    "out vec4 color;"
    "uniform vec4 pickingColor;"
    "void main(void){"
    //     "color = vec4(1.0, 0.0, 0.0, 1.0);}");
    "color = pickingColor;}");

const std::string vRenderSource(
    "#version 400\n"
    "in vec3 inPos;"
    "in vec3 inColor;"
    "out vec3 position;"
    "out vec3 vColor;"
    "uniform mat4 projViewModel;"
    "uniform mat4 viewModel;"
    "void main(void) {"
    "vColor = inColor;"
    "position = (viewModel*vec4(inPos, 1.0)).xyz;"
    "gl_Position = projViewModel * vec4(inPos, 1.0);}");

const std::string gRenderSource(
    "#version 400\n"
    "layout(triangles) in;"
    "layout(triangle_strip, max_vertices = 3) out;"
    "in vec3 position[3];"
    "in vec3 vColor[3];"
    "out vec3 normal;"
    "out vec3 color;"
    "out vec3 gposition;"
    "void main(void) {"
    "vec3 axis0 = normalize(position[1]-position[0]);"
    "vec3 axis1 = normalize(position[2]-position[0]);"
    "normal = cross(axis0, axis1);"
    "gposition = position[0];"
    "color = vColor[0];"
    "gl_Position = gl_in[0].gl_Position;"
    "EmitVertex();"
    "gposition = position[1];"
    "color = vColor[1];"
    "gl_Position = gl_in[1].gl_Position;"
    "EmitVertex();"
    "gposition = position[2];"
    "color = vColor[2];"
    "gl_Position = gl_in[2].gl_Position;"
    "EmitVertex();"
    "EndPrimitive();}");

const std::string fRenderSource(
    "#version 400\n"
    "in vec3 normal;"
    "in vec3 color;"
    "in vec3 gposition;"
    "out vec4 oColor;"
    "void main(void){"
    "vec3 N = normalize( normal );"
    "vec3 L = normalize( -gposition );"
    "float diff = dot( N, L );"
    "diff = clamp( diff, 0.0, 1.0 );"
    "oColor = vec4( diff*color*0.8+color*0.2, 1.0 );}");

Scene::Scene()
    : showFPS(false)
    , _renderMode(SOLID)
    , _framesCount(0)
    , _width(600)
    , _height(600)
{
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    _camera = new Camera();
    _program = new RenderProgram(vRenderSource, gRenderSource, fRenderSource);
    _pickingProgram =
        new RenderProgram(vPickingSource, std::string(""), fPickingSource);
    _previousTime = std::chrono::steady_clock::now();
}

Scene::~Scene()
{
    meshes.clear();
    delete _program;
}

void Scene::render()
{
    ++_framesCount;
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _program->use();
    Eigen::Matrix4f projView = _camera->projectionViewMatrix().cast<float>();
    Eigen::Matrix4f view = _camera->viewMatrix().cast<float>();
    glUniformMatrix4fv(_program->projviewmodelIndex, 1, GL_FALSE,
                       projView.data());
    glUniformMatrix4fv(_program->viewmodelIndex, 1, GL_FALSE, view.data());
    for (auto mesh : meshes)
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

uint32_t Scene::picking(uint32_t x, uint32_t y)
{
    if (_renderMode == WIREFRAME)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glCullFace(GL_FRONT);
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _pickingProgram->use();
    Eigen::Matrix4f projView = _camera->projectionViewMatrix().cast<float>();
    glUniformMatrix4fv(_pickingProgram->projviewmodelIndex, 1, GL_FALSE,
                       projView.data());
    for (uint32_t id = 0; id < meshes.size(); ++id)
    {
        auto mesh = meshes[id];
        float* idColor = _idToColor4f(id + 1);
        glUniform4fv(_pickingProgram->pickingColor, 1, idColor);
        mesh->renderSurface();
    }

    glFlush();
    glFinish();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned char data[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    uint32_t pickedId = (float)data[0] + (float)data[1] * (float)256 +
                        (float)data[2] * 256 * 256;

    if (_renderMode == WIREFRAME)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glCullFace(GL_FRONT_AND_BACK);
    }
    return pickedId;
}

void Scene::cameraRatio(uint32_t width, uint32_t height)
{
    _width = width;
    _height = height;
    _camera->ratio((double)_width / _height);
    glViewport(0, 0, _width, _height);
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

void Scene::updateColors(phyanim::Vec3 staticColor,
                         phyanim::Vec3 dynamicColor,
                         phyanim::Vec3 collideColor)
{
    for (auto mesh : meshes)
    {
        size_t nodesSize = mesh->nodes.size();
        std::vector<double> colorBuffer(nodesSize * 3);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint64_t i = 0; i < nodesSize; ++i)
        {
            auto node = mesh->nodes[i];
            phyanim::Vec3 color = staticColor;
            if (node->collide)
            {
                color = collideColor;
            }
            else if (node->anim)
            {
                color = dynamicColor;
            }
            colorBuffer[i * 3] = color.x();
            colorBuffer[i * 3 + 1] = color.y();
            colorBuffer[i * 3 + 2] = color.z();
        }
        mesh->uploadColors(colorBuffer);
    }
}

float* Scene::_idToColor4f(uint32_t id)
{
    float* value = (float*)malloc(sizeof(float) * 4);
    value[0] = ((id & 0x000000FF) >> 0) / 255.0f;
    value[1] = ((id & 0x0000FF00) >> 8) / 255.0f;
    value[2] = ((id & 0x00FF0000) >> 16) / 255.0f;
    value[3] = 1.0f;
    return value;
}

}  // namespace examples
