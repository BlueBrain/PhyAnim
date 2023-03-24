#include "OverlapCircuitApp.h"

#include <Python.h>

#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"

int main(int argc, char* argv[])
{
    //     PyObject* pInt;

    examples::OverlapCircuitApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
OverlapCircuitApp::OverlapCircuitApp(int argc, char** argv)
    : GLFWApp(argc, argv)
{
    _palette = new ColorPalette(CONTRAST);
}

void OverlapCircuitApp::_actionLoop()
{
    std::string circuitPath;

    _limits.lowerLimit(phyanim::Vec3(-1000, -1000, -1000));
    _limits.upperLimit(phyanim::Vec3(1000, 1000, 1000));

    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        size_t extensionPos;

        if (_args[i].compare("-l") == 0)
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
        else if (_args[i].find(".json") != std::string::npos)
            circuitPath = _args[i];
        else
            ids.push_back(std::stoul(_args[i]));

        // std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }
    _solver = new CollisionSolver(0.001);

    _setCameraPos(_limits);
    Circuit circuit(circuitPath, pop);
    std::cout << "Number of morphologies to load: " << ids.size() << std::endl;
    std::vector<Morpho*> morphologies = circuit.getNeurons(ids, &_limits);
    uint32_t size = morphologies.size();
    std::cout << "Number of morphologies loaded: " << size << std::endl;

    phyanim::HierarchicalAABBs morphoAABBs(size);
    std::vector<phyanim::Edges> edgesSet(size);
    std::vector<phyanim::Nodes> nodesSet(size);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        edgesSet[i] = morphologies[i]->edges;
        resample(edgesSet[i], 0.15);
        removeOutEdges(edgesSet[i], _limits);
        nodesSet[i] = uniqueNodes(edgesSet[i]);
        morphoAABBs[i] = new phyanim::HierarchicalAABB(edgesSet[i]);
    }

    _setMeshes(edgesSet);

    uint32_t totalIters = 0;
    uint32_t cols =
        _solveCollisions(morphoAABBs, edgesSet, nodesSet, _limits, totalIters);

    _setCameraPos(_limits);
}

uint32_t OverlapCircuitApp::_solveCollisions(
    phyanim::HierarchicalAABBs& aabbs,
    std::vector<phyanim::Edges>& edgesSet,
    std::vector<phyanim::Nodes>& nodesSet,
    phyanim::AxisAlignedBoundingBox& limits,
    uint32_t& totalIters)
{
    phyanim::Edges edges;
    for (uint32_t i = 0; i < edgesSet.size(); ++i)
        edges.insert(edges.end(), edgesSet[i].begin(), edgesSet[i].end());
    phyanim::Nodes nodes = phyanim::uniqueNodes(edges);

    std::cout << "Simulation with " << edges.size() << " springs and "
              << nodes.size() << " nodes" << std::endl;

    auto startTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime;
    uint32_t collisions = 1;
    double ks = 1000.0;
    double ksLimit = 0.01;
    uint32_t numIters = 1000;

    while (collisions > 0)
    {
        for (uint32_t iter = 0; iter < numIters; ++iter)
        {
            collisions = _solver->anim(aabbs, edgesSet, nodesSet, limits, ks,
                                       100.0, 0.0);

            if (totalIters % 100 == 0)
            {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
                for (uint32_t i = 0; i < edgesSet.size(); ++i)
                {
                    updateGeometry(_scene->meshes[i], edgesSet[i],
                                   _palette->color(i) * 0.5, _palette->color(i),
                                   _palette->color(i) * 0.2);
                }
                elapsedTime = std::chrono::steady_clock::now() - startTime;
                std::cout << "Iter: " << totalIters
                          << "  Collisions: " << collisions
                          << "  Stiffness: " << ks
                          << "  Time: " << elapsedTime.count() << " seconds."
                          << std::endl;
            }
            totalIters++;
            if (collisions == 0) break;
        }

        ks *= 0.75;
        numIters *= 0.75;
        if (numIters < 100) numIters = 100;
        if (ks < 0.01) break;
    }

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < edgesSet.size(); ++i)
    {
        updateGeometry(_scene->meshes[i], edgesSet[i], _palette->color(i) * 0.5,
                       _palette->color(i), _palette->color(i) * 0.2);
    }

    elapsedTime = std::chrono::steady_clock::now() - startTime;
    std::cout << "Iter: " << totalIters << "  Collisions: " << collisions
              << "  Stiffness: " << ks << "  Time: " << elapsedTime.count()
              << " seconds." << std::endl;

    return collisions;
};

void OverlapCircuitApp::_setMeshes(std::vector<phyanim::Edges>& edgesSet)
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
            generateMesh(edgesSet[i], _palette->color(i) * 0.5,
                         _palette->color(i), _palette->color(i) * 0.2);
    }
}

void OverlapCircuitApp::_mouseButtonCallback(GLFWwindow* window,
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
