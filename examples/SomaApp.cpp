#include "SomaApp.h"

#include <iostream>

#include "Icosphere.h"

namespace examples
{
SomaApp::SomaApp()
    : GLFWApp()
    , _dt(0.003)
    , _kso(1000.0)
    , _ksi(10.0)
    , _ksp(100.0)
    , _kd(1.0)
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
            else if (option.compare("-kso") == 0)
            {
                _kso = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-ksi") == 0)
            {
                _ksi = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-ksp") == 0)
            {
                _ksp = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-kd") == 0)
            {
                _kd = std::atof(argv[i + 1]);
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
    starts.push_back(std::make_pair(phyanim::Vec3(2.2, 0.0, 0.0), 0.4));
    starts.push_back(std::make_pair(phyanim::Vec3(-1.2, 0.0, 0.0), 0.3));
    starts.push_back(std::make_pair(phyanim::Vec3(0.0, 3.0, 1.0), 0.4));
    somaGen = new SomaGenerator(starts, phyanim::Vec3::Zero(), 1.0, _kso, _ksi,
                                _ksp, _kd);
    // somaGen->simulate(_dt, _iters);
    std::cout << "dt: " << _dt << "\nkso: " << _kso << "\nksi: " << _ksi
              << "\nksp: " << _ksp << "\nkd: " << _kd << std::endl;
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
            somaGen->anim(_dt, true);
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
