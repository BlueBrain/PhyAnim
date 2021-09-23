#include "SomaApp.h"

#include <chrono>
#include <iostream>

#include "Icosphere.h"

int main(int argc, char* argv[])
{
    examples::SomaApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
SomaApp::SomaApp(int argc, char** argv) : GLFWApp(argc, argv), _anim(false) {}

void SomaApp::_actionLoop()
{
    double dt = 0.01;
    double ks = 1;
    uint32_t iters = 2000;
    uint32_t iter = 0;

    for (int i = 0; i < _args.size(); ++i)
    {
        std::string option(_args[i]);
        try
        {
            if (option.compare("-dt") == 0)
            {
                ++i;
                dt = std::stof(_args[i]);
            }
            else if (option.compare("-ks") == 0)
            {
                ++i;
                ks = std::stof(_args[i]);
            }
            else if (option.compare("-it") == 0)
            {
                ++i;
                iters = std::stof(_args[i]);
                iter = iters;
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
    starts.push_back(std::make_pair(phyanim::Vec3(3.0, -3.0, 0.0), 0.4));
    starts.push_back(std::make_pair(phyanim::Vec3(-3.0, -3.0, 0.0), 0.4));
    starts.push_back(std::make_pair(phyanim::Vec3(0.0, 3.0, 0.0), 0.4));
    starts.push_back(std::make_pair(phyanim::Vec3(0.0, .0, 3.0), 0.4));
    starts.push_back(std::make_pair(phyanim::Vec3(0.0, .0, -3.0), 0.4));
    auto somaGen =
        new SomaGenerator(starts, phyanim::Vec3::Zero(), 1, dt, ks, 0.7);

    std::chrono::time_point<std::chrono::steady_clock> startTime =
        std::chrono::steady_clock::now();
    somaGen->simulate(iters);

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << iters << " run in: " << elapsedTime.count() << "seconds"
              << std::endl;

    std::cout << "dt: " << dt << "\nks: " << ks << std::endl;
    phyanim::DrawableMesh* mesh = somaGen->mesh();
    phyanim::AxisAlignedBoundingBox limits;
    limits.unite(phyanim::AxisAlignedBoundingBox(mesh->surfaceTriangles));
    _scene->addMesh(mesh);
    _setCameraPos(limits);

    while (true)
    {
        _animMutex.lock();
        bool anim = _anim;
        _animMutex.unlock();
        if (anim)
        {
            endTime = std::chrono::steady_clock::now();
            elapsedTime = endTime - startTime;
            if (elapsedTime.count() >= 0.005)
            {
                ++iter;
                std::cout << "\rIteration: " << iter << std::flush;
                somaGen->anim(true);
                startTime = endTime;
            }
        }
        else
        {
            startTime = std::chrono::steady_clock::now();
        }
    }
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
                _animMutex.lock();
                _anim = !_anim;
                _animMutex.unlock();
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
