#include "CollidingSomas.h"

#include <Python.h>

#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"

int main(int argc, char* argv[])
{
    examples::CollidingSomas app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
CollidingSomas::CollidingSomas(int argc, char** argv) : GLFWApp(argc, argv)
{
    _palette = new ColorPalette(SCALE);
}

void CollidingSomas::_actionLoop()
{
    double dt = 0.001;
    double ks = 1000.0;
    double kd = 0.1;
    double ksc = 1.0;
    uint32_t num = 100;
    RadiusFunc radiusFunc = RadiusFunc::MIN_NEURITES;

    _limits.lowerLimit(phyanim::Vec3(-1000, -1000, -1000));
    _limits.upperLimit(phyanim::Vec3(1000, 1000, 1000));

    std::string circuitPath;
    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        size_t extensionPos;
        if (_args[i].compare("-dt") == 0)
        {
            ++i;
            dt = std::stod(_args[i]);
        }
        else if (_args[i].compare("-ks") == 0)
        {
            ++i;
            ks = std::stod(_args[i]);
        }
        else if (_args[i].compare("-ksc") == 0)
        {
            ++i;
            ksc = std::stod(_args[i]);
        }
        else if (_args[i].compare("-kd") == 0)
        {
            ++i;
            kd = std::stod(_args[i]);
        }
        else if (_args[i].compare("-n") == 0)
        {
            ++i;
            num = std::stoi(_args[i]);
        }
        else if (_args[i].compare("-l") == 0)
        {
            ++i;
            double x = std::stod(_args[i]);
            ++i;
            double y = std::stod(_args[i]);
            ++i;
            double z = std::stod(_args[i]);
            _limits.lowerLimit(phyanim::Vec3(x, y, z));
        }
        else if (_args[i].compare("-u") == 0)
        {
            ++i;
            double x = std::stod(_args[i]);
            ++i;
            double y = std::stod(_args[i]);
            ++i;
            double z = std::stod(_args[i]);
            _limits.upperLimit(phyanim::Vec3(x, y, z));
        }
        else if (_args[i].compare("-p") == 0)
        {
            ++i;
            pop = _args[i];
        }
        else if (_args[i].compare("-minNeurites") == 0)
            radiusFunc = RadiusFunc::MIN_NEURITES;
        else if (_args[i].compare("-maxNeurites") == 0)
            radiusFunc = RadiusFunc::MAX_NEURITES;
        else if (_args[i].compare("-meanNeurites") == 0)
            radiusFunc = RadiusFunc::MEAN_NEURITES;
        else if (_args[i].compare("-minSomas") == 0)
            radiusFunc = RadiusFunc::MIN_SOMAS;
        else if (_args[i].compare("-maxSomas") == 0)
            radiusFunc = RadiusFunc::MAX_SOMAS;
        else if (_args[i].compare("-meanSomas") == 0)
            radiusFunc = RadiusFunc::MEAN_SOMAS;
        else if (_args[i].find(".json") != std::string::npos)
            circuitPath = _args[i];
        else
            ids.push_back(std::stoul(_args[i]));
    }

    _setCameraPos(_limits);
    Circuit circuit(circuitPath, pop);
    std::vector<Morpho*> morphologies =
        circuit.getNeurons(ids, &_limits, radiusFunc, false);

    std::cout << morphologies.size() << " morphologies loaded" << std::endl;

    for (uint32_t i = 0; i < morphologies.size(); ++i)
    {
        auto morpho = morphologies[i];
        if (!morpho->soma)
        {
            delete morpho;
            morphologies.erase(morphologies.begin() + i);
            --i;
        }
        else
        {
            morpho->soma->isSoma = false;
            morpho->soma->collide = false;
        }
    }

    uint32_t size = morphologies.size();
    std::vector<double> factor(size);
    std::vector<phyanim::Edges> edgesSet(size);
    // Check soma collisions

    uint32_t numCollisions = 0;

    for (uint32_t i = 0; i < size; ++i)
    {
        auto soma0 = morphologies[i]->soma;
        for (uint32_t j = i + 1; j < size; ++j)
        {
            auto soma1 = morphologies[j]->soma;
            double dist = (soma1->position - soma0->position).norm() /
                          (soma1->radius + soma0->radius);
            if (dist < 1.0)
            {
                ++numCollisions;
                double f = 1.0 - dist;
                factor[i] = std::max(factor[i], f);
                factor[j] = std::max(factor[j], f);
            }
        }
        edgesSet[i].push_back(new phyanim::Edge(soma0, soma0));
    }

    _setMeshes(edgesSet, factor);
    std::cout << "Number of somas colliding: " << numCollisions << std::endl;

    while (true)
    {
        if (_anim) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    //
    //     while (true)
    //     {
    //         if (_anim)
    //         {
    //             std::cout << "*" << std::flush;
    //             _solver->solve(aabbs, edges, nodes, *aabbs[0], ks, ksc, kd);
    //
    //             updateGeometry(_scene->meshes[0], edges, _palette->color(0) *
    //             0.2,
    //                            _palette->color(0), _palette->color(0) * 0.2);
    //         }
    //     }
}

void CollidingSomas::_setMeshes(std::vector<phyanim::Edges>& edgesSet,
                                std::vector<double> factor)
{
    for (auto mesh : _scene->meshes)
        if (mesh) delete mesh;
    _scene->meshes.clear();
    _scene->meshes.resize(edgesSet.size());

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < edgesSet.size(); ++i)
    {
        _scene->meshes[i] =
            generateMesh(edgesSet[i], _palette->color(factor[i]));
    }
}

void CollidingSomas::_mouseButtonCallback(GLFWwindow* window,
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

}  // namespace examples
