#include <iostream>

#include <GL/glew.h>

#include "GLFWApp.h"


namespace examples
{

GLFWApp::GLFWApp()
    : _scene(nullptr)
    , _mouseX(0.0)
    , _mouseY(0.0)
    , _leftButtonPressed(false)
    , _rightButtonPressed(false)
{
    _initGLFW();
}

GLFWApp::~GLFWApp()
{
    if (_scene)
        delete _scene;
}

void GLFWApp::init(int argc, char** argv)
{
    _scene = new Scene();

    std::string usage = std::string("Usage error: Use ") + std::string(argv[0]) +
            std::string(" mesh_file[.obj|.off]");
    if (argc < 2)
    {
        std::cerr << usage << std::endl;
        exit(-1);
    }
    
    phyanim::DrawableMesh* mesh;
    for (uint32_t i = 1; i < argc; ++i)
    {
        std::string file(argv[i]); 
        mesh = new phyanim::DrawableMesh();
        std::cout << "Loading file " << file << std::endl;
        mesh->load(file);
        mesh->upload();
        _scene->addMesh(mesh);
    }
}

void GLFWApp::loop()
{
    while(!glfwWindowShouldClose(_window))
    {
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void GLFWApp::_initGLFW()
{
    if (!glfwInit())
        throw std::runtime_error("GLFW could not be init");
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); 
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    _window = glfwCreateWindow(600,600,"Example app", NULL, NULL);

    if (!_window) {
        glfwTerminate();
        throw std::runtime_error("GLFW could not create a window");
    }

    glfwMakeContextCurrent(_window);

    glewExperimental = GL_TRUE;
    glewInit();

    auto version = glGetString(GL_VERSION);    

    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, GLFWApp::_wrapperKeyCallback);
    glfwSetWindowSizeCallback(_window, GLFWApp::_wrapperResizeCallback);
    glfwSetMouseButtonCallback(_window, GLFWApp::_wrapperMouseButtonCallback);
    glfwSetCursorPosCallback(_window, GLFWApp::_wrapperMousePositionCallback);
    
}


void GLFWApp::_wrapperKeyCallback(GLFWwindow* window, int key, int scancode,
                                int action, int mods)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_keyCallback(window, key, scancode, action, mods);
}

void GLFWApp::_keyCallback(GLFWwindow* window, int key, int scancode, int action,
                         int mods)
{
    if (_scene)
    {
        double dx = 0.1;
        phyanim::Vec3 dxyz(0.0, 0.0, 0.0);
        bool cameraDisplaced = false;
    
        if (action == GLFW_PRESS) {
            switch(key) {
            case 'M':
                _scene->changeRenderMode();
                break;
            case 'F':
                _scene->showFPS = !_scene->showFPS;
                break;
            }
        }
    
        switch(key){
        case 'W':
            dxyz += phyanim::Vec3(0.0, 0.0, -dx);
            cameraDisplaced = true;
            break;
        case 'S':
            dxyz += phyanim::Vec3(0.0, 0.0, dx);
            cameraDisplaced = true;
            break;
        case 'A':
            dxyz += phyanim::Vec3(-dx, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        case 'D':
            dxyz += phyanim::Vec3(dx, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        }
        if (cameraDisplaced) {
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

void GLFWApp::_wrapperMouseButtonCallback(GLFWwindow* window, int button,
                                          int action, int mods)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_mouseButtonCallback(window, button, action, mods);
}

void GLFWApp::_mouseButtonCallback(GLFWwindow* window, int button, int action,
                                   int mods)
{
    if (_scene)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                _leftButtonPressed = true;
            
                glfwGetCursorPos(window, &_mouseX, &_mouseY);
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                _rightButtonPressed = true;
            
                glfwGetCursorPos(window, &_mouseX, &_mouseY);
            }
            else if (action == GLFW_RELEASE)
            {
                _rightButtonPressed = false;
            }   
        }
    }
}

void GLFWApp::_wrapperMousePositionCallback(GLFWwindow* window, double xpos,
                                            double ypos)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_mousePositionCallback(window, xpos, ypos);
}

void GLFWApp::_mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (_scene)
    {
        if (_rightButtonPressed)
        {
            double diffX = xpos - _mouseX; 
            double diffY = ypos - _mouseY;
            _mouseX = xpos;
            _mouseY = ypos;
            _scene->rotateCamera(-diffY*0.005, -diffX*0.005);
        }
        if (_leftButtonPressed)
        {
            double diffX = xpos - _mouseX; 
            double diffY = ypos - _mouseY;
            _mouseX = xpos;
            _mouseY = ypos;
            phyanim::Vec3 dxyz = phyanim::Vec3(-diffX*0.01, diffY*0.01, 0.0);
            _scene->displaceCamera(dxyz);
        }
    }
}

}
