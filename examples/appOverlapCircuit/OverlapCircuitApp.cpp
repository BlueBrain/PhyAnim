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

    double dt = 0.001;
    double ks = 1000.0;
    double mult = 0.5;
    double kd = 0.0;
    double ksc = 100.0;
    double segmentLen = 0.2;

    uint32_t num = 1000;
    double iterMult = 0.75;

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
        else if (_args[i].compare("-ksf") == 0)
        {
            ++i;
            mult = std::stod(_args[i]);
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
        else if (_args[i].compare("-it") == 0)
        {
            ++i;
            num = std::stoi(_args[i]);
        }
        else if (_args[i].compare("-itf") == 0)
        {
            ++i;
            iterMult = std::stod(_args[i]);
        }
        else if (_args[i].compare("-len") == 0)
        {
            ++i;
            segmentLen = std::stod(_args[i]);
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
        else if (_args[i].find(".json") != std::string::npos)
            circuitPath = _args[i];
        else
            ids.push_back(std::stoul(_args[i]));

        // std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }
    _solver = new CollisionSolver(dt);

    _setCameraPos(_limits);
    Circuit circuit(circuitPath, pop);
    std::cout << "Number of morphologies to load: " << ids.size() << std::endl;
    std::vector<Morpho*> morphologies = circuit.getNeurons(ids, &_limits);
    uint32_t size = morphologies.size();
    std::cout << "Number of morphologies loaded: " << size << std::endl;

    phyanim::HierarchicalAABBs morphoAABBs(size);
    std::vector<phyanim::Edges> edgesSet(size);
    std::vector<phyanim::Nodes> nodesSet(size);

    uint32_t sizeEdges = 0;
    uint32_t sizeNodes = 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        edgesSet[i] = morphologies[i]->edges;
        // resample(edgesSet[i]);
        resample(edgesSet[i], segmentLen);
        removeOutEdges(edgesSet[i], _limits);
        nodesSet[i] = uniqueNodes(edgesSet[i]);
        // _limits.fixOutNodes(nodesSet[i]);
        morphoAABBs[i] = new phyanim::HierarchicalAABB(edgesSet[i]);
#ifdef PHYANIM_USES_OPENMP
#pragma omp critical
#endif
        {
            sizeEdges += edgesSet[i].size();
            sizeNodes += nodesSet[i].size();
        }
    }

    std::cout << "Simulation with " << sizeEdges << " springs and " << sizeNodes
              << " nodes" << std::endl;

    _setMeshes(edgesSet);

    _anim = true;
    while (true)
    {
        if (_anim) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    auto startTime = std::chrono::steady_clock::now();

    uint32_t cols = 1;
    uint32_t totalIters = 0;
    for (uint32_t iter = 0; iter < 10000; ++iter)
    {
        
        cols = _solveCollisions(morphoAABBs, edgesSet, nodesSet, _limits, ks,
                                ksc, kd, totalIters, num);
        ks *= mult;
        num *= iterMult;
        if (num < 100) num = 100;
        if (ks < 0.01) ks = 0;
        if (cols == 0) break;
    }
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;

    std::cout << "Solved in " << totalIters << " iters and " << elapsedTime.count() << " seconds."<<  std::endl;

    _setCameraPos(_limits);
}

uint32_t OverlapCircuitApp::_solveCollisions(
    phyanim::HierarchicalAABBs& aabbs,
    std::vector<phyanim::Edges>& edgesSet,
    std::vector<phyanim::Nodes>& nodesSet,
    phyanim::AxisAlignedBoundingBox& limits,
    double ks,
    double ksc,
    double kd,
    uint32_t& totalIters,
    uint32_t numIters)
{
    uint32_t size = aabbs.size();
    uint32_t cols;

    for (uint32_t iter = 0; iter < numIters; ++iter)
    {
        if (!_anim)
        {
            --iter;
            continue;
        }

        cols = _solver->solve(aabbs, edgesSet, nodesSet, limits, ks, ksc, kd);

        if (totalIters % 100 == 0)
        {
            std::cout << "Iter: " << totalIters << " collisions: " << cols
                      << " stiffness: " << ks << std::endl;
        }
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < size; ++i)
        {
            updateGeometry(_scene->meshes[i], edgesSet[i],
                           _palette->color(i) * 0.5, _palette->color(i),
                           _palette->color(i) * 0.2);
        }
        totalIters++;
        if (cols == 0)
        {
            return 0;
        }
    }
    return cols;
}

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
            generateMesh(edgesSet[i], _palette->color(i), _palette->color(i),
                         _palette->color(i) * 0.2);
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
