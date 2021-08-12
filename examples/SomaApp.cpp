#include "SomaApp.h"

#include <iostream>

#include "Icosphere.h"

namespace examples
{
SomaApp::SomaApp()
    : GLFWApp()
    , _dt(0.01)
    , _ks(0.5)
    , _iters(1000)
    , _anim(false)
    , _iter(0)
{
}

void SomaApp::init(int argc, char** argv)
{
    _scene = new Scene();

    for (int i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);
        try
        {
            if (option.compare("-dt") == 0)
            {
                _dt = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-ks") == 0)
            {
                _ks = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-it") == 0)
            {
                _iters = std::atof(argv[i + 1]);
                ++i;
            }
        }
        catch (...)
        {
            std::cerr << "Usage error\n" << std::endl;
            exit(-1);
        }
    }

    NeuriteStarts starts;
    starts.push_back(std::make_pair(phyanim::Vec3(3.2, -3.0, 0.0), 0.04));
    starts.push_back(std::make_pair(phyanim::Vec3(-3.0, -3.0, 0.0), 0.03));
    starts.push_back(std::make_pair(phyanim::Vec3(0.0, 3.0, 0.0), 0.04));
    starts.push_back(std::make_pair(phyanim::Vec3(0.0, .0, 3.0), 0.04));
    starts.push_back(std::make_pair(phyanim::Vec3(0.0, .0, -3.0), 0.04));
    somaGen =
        new SomaGenerator(starts, phyanim::Vec3::Zero(), 0.1, _dt, _ks, 0.7);
    // somaGen->simulate(_dt, _iters);
    std::cout << "dt: " << _dt << "\nks: " << _ks << std::endl;
    phyanim::DrawableMesh* mesh;
    phyanim::AxisAlignedBoundingBox limits;
    mesh = somaGen->mesh();
    mesh->upload();
    limits.unite(phyanim::AxisAlignedBoundingBox(mesh->surfaceTriangles));
    _scene->addMesh(mesh);
    _setCameraPos(limits);
}

void SomaApp::loop()
{
    while (!glfwWindowShouldClose(_window))
    {
        if (_anim)
        {
            ++_iter;
            std::cout << "\rIteration: " << _iter << std::flush;
            somaGen->anim(true);
        }
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void SomaApp::_keyCallback(GLFWwindow* window,
                           int key,
                           int scancode,
                           int action,
                           int mods)
{
    GLFWApp::_keyCallback(window, key, scancode, action, mods);
    if (_scene)
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case GLFW_KEY_SPACE:
                _anim = !_anim;
                if (_anim)
                    std::cout << "\nsimulation released" << std::endl;
                else
                    std::cout << "\nsimulation paused" << std::endl;
                break;
            }
        }
    }
}

}  // namespace examples
