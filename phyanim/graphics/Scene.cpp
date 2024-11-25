/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Scene.h"

#include <GL/glew.h>

#include <cstdlib>
#include <iostream>

namespace phyanim
{
namespace graphics
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
    "color = pickingColor;}");

const std::string vRenderSource(
    "#version 400\n"
    "in vec3 inPos;"
    "in vec3 inNormal;"
    "in vec3 inColor;"
    "out vec3 position;"
    "out vec3 normal;"
    "out vec3 color;"
    "uniform mat4 projViewModel;"
    "uniform mat4 viewModel;"
    "void main(void) {"
    "color = inColor;"
    "position = (viewModel*vec4(inPos, 1.0)).xyz;"
    "normal = (viewModel*vec4(inNormal, 0.0)).xyz;"
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
    "normal = cross(axis0, axis1);"
    "gposition = position[1];"
    "color = vColor[1];"
    "gl_Position = gl_in[1].gl_Position;"
    "EmitVertex();"
    "normal = cross(axis0, axis1);"
    "gposition = position[2];"
    "color = vColor[2];"
    "gl_Position = gl_in[2].gl_Position;"
    "EmitVertex();"
    "EndPrimitive();}");

const std::string fRenderSource(
    "#version 400\n"
    "in vec3 position;"
    "in vec3 normal;"
    "in vec3 color;"
    "out vec4 oColor;"
    "void main(void){"
    "vec3 N = normalize( normal );"
    "vec3 L = normalize( -position );"
    "float diff = dot( N, L );"
    "diff = clamp( diff, 0.0, 1.0 );"
    "oColor = vec4( diff*color*0.8+color*0.2, 1.0 );}");

Scene::Scene(uint32_t width, uint32_t height)
    : showFPS(false)
    , _renderMode(SOLID)
    , _framesCount(0)
    , _width(width)
    , _height(height)
    , _background(0.8f, 0.8f, 0.8f)
    , _genSky(false)
    , _sky(nullptr)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    _camera = new Camera(geometry::Vec3(), geometry::Mat3(1.0f), 1.0f, 90.0,
                         (float)_width / _height);
    _program = new RenderProgram(vRenderSource, "", fRenderSource);
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
    glClearColor(_background.x, _background.y, _background.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (_renderMode)
    {
    case SOLID:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        break;
    case WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        break;
    }

    _loadSky();

    if (_sky) _sky->render(_camera);

    _program->use();
    glUniformMatrix4fv(_program->projviewmodelIndex, 1, GL_FALSE,
                       glm::value_ptr(_camera->projectionViewMatrix()));
    glUniformMatrix4fv(_program->viewmodelIndex, 1, GL_FALSE,
                       glm::value_ptr(_camera->viewMatrix()));
    for (auto mesh : meshes)
    {
        if (mesh) mesh->render();
    }
    auto currentTime = std::chrono::steady_clock::now();
    float elapsed_seconds =
        (std::chrono::duration<float>(currentTime - _previousTime)).count();
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _pickingProgram->use();
    glUniformMatrix4fv(_pickingProgram->projviewmodelIndex, 1, GL_FALSE,
                       glm::value_ptr(_camera->projectionViewMatrix()));
    for (uint32_t id = 0; id < meshes.size(); ++id)
    {
        auto mesh = meshes[id];
        float* idColor = _idToColor4f(id + 1);
        glUniform4fv(_pickingProgram->pickingColor, 1, idColor);
        if (mesh) mesh->render();
    }

    glFlush();
    glFinish();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned char data[4];
    glReadPixels(x, _height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    uint32_t pickedId = (float)data[0] + (float)data[1] * (float)256 +
                        (float)data[2] * 256 * 256;

    return pickedId;
}

void Scene::cameraRatio(uint32_t width, uint32_t height)
{
    _width = width;
    _height = height;
    _camera->ratio((float)_width / _height);
    glViewport(0, 0, _width, _height);
}

void Scene::cameraPosition(geometry::Vec3 position)
{
    _camera->position(position);
}

void Scene::cameraDistance(float distance) { _camera->distance(distance); }

float Scene::cameraDistance() { return _camera->distance(); }

void Scene::cameraFov(float fov) { _camera->fov(fov); }

void Scene::displaceCamera(geometry::Vec3 displace)
{
    geometry::Vec3 rotDis = displace * _camera->rotation();
    _camera->position(_camera->position() + rotDis);
}

void Scene::cameraZoom(float zoomInOut)
{
    if (zoomInOut > 0)
        _camera->distance(_camera->distance() * 1.1f);
    else
        _camera->distance(_camera->distance() * 0.9f);
}

void Scene::rotateCamera(float pitch, float yaw)
{
    float cosX = cos(pitch);
    float sinX = sin(pitch);
    float cosY = cos(yaw);
    float sinY = sin(yaw);
    geometry::Mat3 rotX = {1.0, 0.0, 0.0, 0.0, cosX, -sinX, 0.0, sinX, cosX};
    geometry::Mat3 rotY = {cosY, 0.0, sinY, 0.0, 1.0, 0.0, -sinY, 0.0, cosY};
    geometry::Mat3 rot = rotX * rotY;
    _camera->rotation(rot * _camera->rotation());
}

void Scene::changeRenderMode()
{
    switch (_renderMode)
    {
    case SOLID:
        _renderMode = WIREFRAME;
        break;
    case WIREFRAME:
        _renderMode = SOLID;
        break;
    }
}

geometry::Mat3 Scene::cameraRotation() const { return _camera->rotation(); }

void Scene::setSky(const std::string& file)
{
    _skyPath = file;
    _genSky = true;
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

void Scene::_loadSky()
{
    if (_genSky)
    {
        if (!_skyPath.empty())
        {
            if (_sky) delete _sky;
            _sky = new SkyBox(_skyPath);
            _genSky = false;
        }
    }
}

}  // namespace graphics
}  // namespace phyanim
