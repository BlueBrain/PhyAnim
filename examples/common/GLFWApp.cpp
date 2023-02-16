#ifdef PHYANIM_USES_OPENMP

// clang-format off
#include <GL/glew.h>
// clang-format on

#include "GLFWApp.h"

#include <iomanip>
#include <iostream>
#include <thread>

namespace examples
{
GLFWApp::GLFWApp(int argc, char** argv)
    : _scene(nullptr)
    , _width(1000)
    , _height(1000)
    , _mouseX(0.0)
    , _mouseY(0.0)
    , _leftButtonPressed(false)
    , _middleButtonPressed(false)
    , _rightButtonPressed(false)
    , _anim(false)
    , _collisionId(0)
    , _bbFactor(15.0)
{
    for (uint32_t i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg.compare("-w") == 0)
        {
            ++i;
            _width = std::stoi(argv[i]);
        }
        else if (arg.compare("-h") == 0)
        {
            ++i;
            _height = std::stoi(argv[i]);
        }
        else
            _args.push_back(std::string(argv[i]));
    }
    _initGLFW();
    _scene = new Scene(_width, _height);
}

GLFWApp::~GLFWApp()
{
    if (_scene) delete _scene;
}

void GLFWApp::run()
{
    std::thread actionTask(_actionThread, this);

    _renderLoop();
    actionTask.detach();
}

void GLFWApp::_renderLoop()
{
    while (!glfwWindowShouldClose(_window))
    {
        glfwMakeContextCurrent(_window);
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void GLFWApp::_actionLoop()
{
    std::vector<std::string> files;
    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        if (_args[i].compare("-bb") == 0)
        {
            ++i;
            _bbFactor = std::stoi(_args[i]);
        }
        else
        {
            files.push_back(_args[i]);
        }
    }
    _loadMeshes(files);
    std::cout << "Number of collisions: " << _aabbs.size() << std::endl;
}

void GLFWApp::_actionThread(GLFWApp* app) { app->_actionLoop(); }

void GLFWApp::_loadMeshes(std::vector<std::string>& files,
                          double stiffnes,
                          double density,
                          double damping,
                          double poissonRatio)
{
    std::cout << std::fixed << std::setprecision(2);
    double progress = 0.0;
    std::cout << "\rLoading files " << progress << "%" << std::flush;

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        auto mesh = new phyanim::Mesh(stiffnes, density, damping, poissonRatio);
        mesh->load(files[i]);
        auto renderMesh = generateMesh(mesh->nodes, mesh->surfaceTriangles);
        mesh->boundingBox =
            new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
#pragma omp critical
        {
            _limits.unite(*mesh->boundingBox);
            _meshes.push_back(mesh);
            _scene->meshes.push_back(renderMesh);
            _setCameraPos(_limits);
            progress += 100.0f / files.size();
            std::cout << "\rLoading files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
    _aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(_meshes, _bbFactor);
    _sortAABBs(_aabbs);
    std::cout << "Number of collisions: " << _aabbs.size() << std::endl;
    _collisionId = 0;
}

void GLFWApp::_writeMeshes(phyanim::Meshes& meshes,
                           std::vector<std::string>& files,
                           std::string extension)
{
    double progress = 0.0;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\rSaving files " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < meshes.size(); ++i)
    {
        auto outFile = files[i];
        uint32_t pos = outFile.find_last_of('/');
        if (pos != std::string::npos) outFile = outFile.substr(pos + 1);
        pos = outFile.find(".tet");
        if (pos != std::string::npos) outFile = outFile.substr(0, pos);
        outFile += extension + ".tet";
        meshes[i]->write(outFile);
#pragma omp critical
        {
            progress += 100.0f / meshes.size();
            std::cout << "\rSaving files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Files saved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

bool GLFWApp::_getAnim()
{
    _animMutex.lock();
    bool anim = _anim;
    _animMutex.unlock();
    return anim;
}

void GLFWApp::_setAnim(bool anim)
{
    _animMutex.lock();
    _anim = anim;
    _animMutex.unlock();
}

void GLFWApp::_setCameraPos(phyanim::AxisAlignedBoundingBox limits)
{
    phyanim::Vec3 cameraPos = limits.center();
    float distance = limits.radius();
    _scene->cameraPosition(cameraPos);
    _scene->cameraDistance(distance);
}

void GLFWApp::_initGLFW()
{
    glfwSetErrorCallback(&glfwError);

    if (!glfwInit()) throw std::runtime_error("GLFW could not be init");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    _window = glfwCreateWindow(_width, _height, "Example app", NULL, NULL);

    if (!_window)
    {
        glfwTerminate();
        throw std::runtime_error("GLFW could not create a window");
    }

    glfwMakeContextCurrent(_window);
    glfwSwapInterval(0);

    glewExperimental = GL_TRUE;
    glewInit();

    auto version = glGetString(GL_VERSION);

    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, GLFWApp::_wrapperKeyCallback);
    glfwSetWindowSizeCallback(_window, GLFWApp::_wrapperResizeCallback);
    glfwSetMouseButtonCallback(_window, GLFWApp::_wrapperMouseButtonCallback);
    glfwSetCursorPosCallback(_window, GLFWApp::_wrapperMousePositionCallback);
    glfwSetScrollCallback(_window, _wrapperMouseScrollCallback);
}

void GLFWApp::_wrapperKeyCallback(GLFWwindow* window,
                                  int key,
                                  int scancode,
                                  int action,
                                  int mods)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_keyCallback(window, key, scancode, action, mods);
}

void GLFWApp::_keyCallback(GLFWwindow* window,
                           int key,
                           int scancode,
                           int action,
                           int mods)
{
    if (_scene)
    {
        phyanim::Vec3 dxyz(0.0, 0.0, 0.0);
        bool cameraDisplaced = false;

        if (action == GLFW_PRESS)
        {
            uint32_t aabbsSize = _aabbs.size();
            switch (key)
            {
            case 'M':
                _scene->changeRenderMode();
                break;
            case 'F':
                _scene->showFPS = !_scene->showFPS;
                break;
            case GLFW_KEY_SPACE:
                _setAnim(!_anim);
                if (_anim)
                    std::cout << "release" << std::endl;
                else
                    std::cout << "pause" << std::endl;
                break;
            case GLFW_KEY_LEFT:
                if (aabbsSize > 0)
                {
                    _collisionId = (_collisionId - 1);
                    if (_collisionId < 0) _collisionId = aabbsSize - 1;
                    std::cout << "Collision id: " << _collisionId << std::endl;
                    auto limit = *_aabbs[_collisionId];
                    limit.resize(2.0f);
                    _setCameraPos(limit);
                }
                break;
            case GLFW_KEY_RIGHT:
                if (aabbsSize > 0)
                {
                    _collisionId = (_collisionId + 1) % aabbsSize;
                    std::cout << "Collision id: " << _collisionId << std::endl;
                    auto limit = *_aabbs[_collisionId];
                    limit.resize(2.0f);
                    _setCameraPos(limit);
                }
                break;
            }
        }
        switch (key)
        {
        case 'W':
            dxyz += phyanim::Vec3(0.0, 0.0, -1.0);
            cameraDisplaced = true;
            break;
        case 'S':
            dxyz += phyanim::Vec3(0.0, 0.0, 1.0);
            cameraDisplaced = true;
            break;
        case 'A':
            dxyz += phyanim::Vec3(-1.0, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        case 'D':
            dxyz += phyanim::Vec3(1.0, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        }

        if (cameraDisplaced)
        {
            dxyz *= _scene->cameraDistance() * 0.001f * 10.0;
            _scene->displaceCamera(dxyz);
        }
    }
}

void GLFWApp::_wrapperResizeCallback(GLFWwindow* window, int width, int height)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_resizeCallback(window, width, height);
}

void GLFWApp::_resizeCallback(GLFWwindow* window, int width, int height)
{
    if (_scene)
    {
        _scene->cameraRatio(width, height);
    }
}

void GLFWApp::_wrapperMouseButtonCallback(GLFWwindow* window,
                                          int button,
                                          int action,
                                          int mods)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_mouseButtonCallback(window, button, action, mods);
}

void GLFWApp::_mouseButtonCallback(GLFWwindow* window,
                                   int button,
                                   int action,
                                   int mods)
{
    if (_scene)
    {
        glfwGetCursorPos(window, &_mouseX, &_mouseY);

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                _leftButtonPressed = true;

                uint32_t pickedId = _scene->picking(_mouseX, _mouseY);

                for (uint32_t id = 0; id < _scene->meshes.size(); ++id)
                {
                    phyanim::Vec3 color(0, 0.8, 0.8);
                    if (id + 1 == pickedId)
                        color = phyanim::Vec3(0.8, 0.4, 0.4);
                    setColor(_scene->meshes[id], _meshes[id]->nodes.size(),
                             color);
                }
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if (action == GLFW_PRESS)
            {
                _middleButtonPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                _middleButtonPressed = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                _rightButtonPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                _rightButtonPressed = false;
            }
        }
    }
}

void GLFWApp::_wrapperMousePositionCallback(GLFWwindow* window,
                                            double xpos,
                                            double ypos)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_mousePositionCallback(window, xpos, ypos);
}

void GLFWApp::_mousePositionCallback(GLFWwindow* window,
                                     double xpos,
                                     double ypos)
{
    if (_scene)
    {
        double diffX = xpos - _mouseX;
        double diffY = ypos - _mouseY;
        _mouseX = xpos;
        _mouseY = ypos;
        phyanim::Vec3 dxyz =
            phyanim::Vec3(-diffX * _scene->cameraDistance() * 0.001f,
                          diffY * _scene->cameraDistance() * 0.001f, 0.0);
        if (_leftButtonPressed)
        {
            _scene->displaceCamera(dxyz);
        }
        if (_middleButtonPressed)
        {
        }
        if (_rightButtonPressed)
        {
            _scene->rotateCamera(-diffY * 0.005, -diffX * 0.005);
        }
    }
}

void GLFWApp::_wrapperMouseScrollCallback(GLFWwindow* window,
                                          double xoffset,
                                          double yoffset)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_mouseScrollCallback(window, xoffset, yoffset);
}

void GLFWApp::_mouseScrollCallback(GLFWwindow* window,
                                   double xoffset,
                                   double yoffset)
{
    if (_scene) _scene->cameraZoom(-yoffset);
}

}  // namespace examples

#endif