/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <chrono>
#include <mutex>

#include "Camera.h"
#include "Mesh.h"
#include "RenderProgram.h"
#include "SkyBox.h"

namespace phyanim
{
namespace graphics
{
class Scene
{
public:
    Scene(uint32_t width, uint32_t height);

    ~Scene();

    void addMesh(Mesh* mesh);

    void render();

    uint32_t picking(uint32_t x, uint32_t y);

    void cameraRatio(uint32_t width, uint32_t height);

    void cameraPosition(geometry::Vec3 position);

    void cameraDistance(float distance);

    float cameraDistance();

    void cameraFov(float fov);

    void displaceCamera(geometry::Vec3 displace);

    void cameraZoom(float zoomInOut);

    void rotateCamera(float pitch, float yaw);

    void changeRenderMode();

    geometry::Mat3 cameraRotation() const;

    void setSky(const std::string& file);

private:
    float* _idToColor4f(uint32_t id);

    void _loadSky();

public:
    bool showFPS;

    std::vector<Mesh*> meshes;

private:
    typedef enum RenderMode
    {
        SOLID = 0,
        WIREFRAME
    } RenderMode;

    Camera* _camera;

    std::string _skyPath;
    bool _genSky;
    SkyBox* _sky;

    geometry::Vec3 _background;

    RenderMode _renderMode;
    bool _renderModeChanged;

    RenderProgram* _program;
    RenderProgram* _pickingProgram;

    uint32_t _width;
    uint32_t _height;

    uint64_t _framesCount;

    std::chrono::time_point<std::chrono::steady_clock> _previousTime;
};

}  // namespace graphics
}  // namespace phyanim

#endif
