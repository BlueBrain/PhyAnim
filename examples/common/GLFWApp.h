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
 
#ifndef __EXAMPLES_GLFW_APP__
#define __EXAMPLES_GLFW_APP__

#ifdef PHYANIM_USES_GLFW3

#include <GLFW/glfw3.h>
#include <phyanim/Phyanim.h>

using namespace phyanim;

namespace examples
{
class GLFWApp
{
public:
    GLFWApp(int argc, char** argv);

    virtual ~GLFWApp();

    void run();

protected:
    void _renderLoop();

    virtual void _actionLoop();

    static void _actionThread(GLFWApp* app);

    void _loadMeshes(std::vector<std::string>& files,
                     float stiffnes = 1000.0,
                     float density = 1.0,
                     float damping = 1.0,
                     float poissonRatio = 0.49);

    void _writeMeshes(geometry::Meshes& meshes,
                      std::vector<std::string>& files,
                      std::string extension = "");

    void _sortAABBs(geometry::AxisAlignedBoundingBoxes& aabbs)
    {
        auto cmp = [](geometry::AxisAlignedBoundingBoxPtr a,
                      geometry::AxisAlignedBoundingBoxPtr b) {
            return a->radius() > b->radius();
        };
        std::sort(aabbs.begin(), aabbs.end(), cmp);
    }

    bool _getAnim();

    void _setAnim(bool anim);

    void _setCameraPos(geometry::AxisAlignedBoundingBox limits);

    virtual void _keyCallback(GLFWwindow* window,
                              int key,
                              int scancode,
                              int action,
                              int mods);

    virtual void _resizeCallback(GLFWwindow* window, int width, int height);

    virtual void _mouseButtonCallback(GLFWwindow* window,
                                      int button,
                                      int action,
                                      int mods);

    virtual void _mousePositionCallback(GLFWwindow* window,
                                        float xpos,
                                        float ypos);

    virtual void _mouseScrollCallback(GLFWwindow* window,
                                      float xoffset,
                                      float yoffset);

private:
    void _initGLFW();

    static void _wrapperKeyCallback(GLFWwindow* window,
                                    int key,
                                    int scancode,
                                    int action,
                                    int mods);

    static void _wrapperResizeCallback(GLFWwindow* window,
                                       int width,
                                       int height);

    static void _wrapperMouseButtonCallback(GLFWwindow* window,
                                            int button,
                                            int action,
                                            int mods);

    static void _wrapperMousePositionCallback(GLFWwindow* window,
                                              double xpos,
                                              double ypos);

    static void _wrapperMouseScrollCallback(GLFWwindow* window,
                                            double xoffset,
                                            double yoffset);

    static void glfwError(int id, const char* description)
    {
        std::cout << description << std::endl;
    }

protected:
    GLFWwindow* _window;

    graphics::Scene* _scene;
    geometry::Meshes _meshes;

    uint32_t _width;
    uint32_t _height;

    std::vector<std::string> _args;

    float _mouseX;
    float _mouseY;

    bool _leftButtonPressed;
    bool _middleButtonPressed;
    bool _rightButtonPressed;

    bool _anim;
    std::mutex _animMutex;

    geometry::AxisAlignedBoundingBox _limits;
    geometry::AxisAlignedBoundingBoxes _aabbs;
    int32_t _collisionId;
    float _bbFactor;
};

}  // namespace examples

#endif

#endif
