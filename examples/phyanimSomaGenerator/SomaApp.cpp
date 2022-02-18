#include "SomaApp.h"

#include <chrono>
#include <iostream>

#include "Icosphere.h"
#include "SWCReader.h"

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
    double stiffness = 10000;
    double poissonRatio = 0.2;
    double alphaSoma = 0.75;
    uint32_t iters = 201;
    uint32_t iter = 0;
    bool offline = false;
    std::string file("data/a_s.swc");

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
                stiffness = std::stof(_args[i]);
            }
            else if (option.compare("-as") == 0)
            {
                ++i;
                alphaSoma = std::stof(_args[i]);
            }
            else if (option.compare("-pr") == 0)
            {
                ++i;
                poissonRatio = std::stof(_args[i]);
            }
            else if (option.compare("-it") == 0)
            {
                ++i;
                iters = std::stof(_args[i]);
            }
            else if (option.compare("-off") == 0)
            {
                offline = true;
            }
            else
            {
                file = option;
            }
        }
        catch (...)
        {
            std::cerr << "Usage error\n" << std::endl;
            exit(-1);
        }
    }

    SWCReader reader(file);

    std::cout << "Soma center " << reader.soma.position << std::endl;

    std::cout << "Soma radius " << reader.soma.radius << std::endl;

    std::chrono::time_point<std::chrono::steady_clock> startTime =
        std::chrono::steady_clock::now();
    auto somaGen = new SomaGenerator(reader.neurites, reader.soma, dt,
                                     stiffness, poissonRatio, alphaSoma);

    std::cout << "dt: " << dt << std::endl;
    std::cout << "stiffness: " << stiffness << std::endl;
    std::cout << "poissonRatio: " << poissonRatio << std::endl;
    std::cout << "alphaSoma: " << alphaSoma << std::endl;
    phyanim::DrawableMesh* mesh = somaGen->mesh();
    phyanim::AxisAlignedBoundingBox limits;
    limits.unite(phyanim::AxisAlignedBoundingBox(mesh->surfaceTriangles));
    phyanim::Vec3 center = limits.center();
    phyanim::Vec3 axis = (limits.upperLimit() - center) * 2.0;

    limits.upperLimit(center + axis);
    limits.lowerLimit(center - axis);
    _scene->addMesh(mesh);
    _setCameraPos(limits);

    _anim = true;

    while (true)
    {
        _animMutex.lock();
        bool anim = _anim;
        _animMutex.unlock();
        if (_anim)
        {
            ++iter;
            if (iter < iters)
            {
                float alpha = (float)iter / iters;
                somaGen->pull(alpha);
                somaGen->anim(!offline);
                if (iter == 1 || iter == 50 || iter == 100 || iter == 150 ||
                    iter == 200)
                {
                    std::string outFile("out_");
                    outFile.append(std::to_string(iter));
                    outFile.append(".obj");
                    mesh->write(outFile);
                }
            }
            else
            {
                somaGen->anim(true);
                if (iter == iters)
                {
                    _anim = false;
                    auto endTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsedTime =
                        endTime - startTime;
                    std::cout << iters
                              << " irations in: " << elapsedTime.count()
                              << "seconds" << std::endl;
                }
                std::cout << "Iteration: " << iter << std::endl;
            }
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
