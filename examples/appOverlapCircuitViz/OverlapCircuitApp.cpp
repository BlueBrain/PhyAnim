#include "OverlapCircuitApp.h"

#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"

using namespace phyanim;

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
    , _threshold(1.0f)
    , _ks(1000.0f)
    , _ksc(100.0f)
    , _ksLimit(0.0001f)
    , _dt(0.0001f)

{
    _palette = new graphics::ColorPalette(graphics::CONTRAST);
}

void OverlapCircuitApp::_actionLoop()
{
    std::string circuitPath;

    _limits.lowerLimit(geometry::Vec3(-1000, -1000, -1000));
    _limits.upperLimit(geometry::Vec3(1000, 1000, 1000));

    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        size_t extensionPos;

        if (_args[i].compare("-l") == 0)
        {
            ++i;
            float x = std::stod(_args[i]);
            ++i;
            float y = std::stod(_args[i]);
            ++i;
            float z = std::stod(_args[i]);
            _limits.lowerLimit(geometry::Vec3(x, y, z));
        }
        else if (_args[i].compare("-u") == 0)
        {
            ++i;
            float x = std::stod(_args[i]);
            ++i;
            float y = std::stod(_args[i]);
            ++i;
            float z = std::stod(_args[i]);
            _limits.upperLimit(geometry::Vec3(x, y, z));
        }
        else if (_args[i].compare("-p") == 0)
        {
            ++i;
            pop = _args[i];
        }
        else if (_args[i].compare("-t") == 0)
        {
            ++i;
            _threshold = std::stof(_args[i]);
        }
        else if (_args[i].compare("-ks") == 0)
        {
            ++i;
            _ks = std::stof(_args[i]);
        }
        else if (_args[i].compare("-ksc") == 0)
        {
            ++i;
            _ksc = std::stof(_args[i]);
        }
        else if (_args[i].compare("-dt") == 0)
        {
            ++i;
            _dt = std::stof(_args[i]);
        }
        else if (_args[i].find(".json") != std::string::npos)
            circuitPath = _args[i];
        else
            ids.push_back(std::stoul(_args[i]));

        // std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }
    _solver = new CollisionSolver(_dt);

    _setCameraPos(_limits);
    Circuit circuit(circuitPath, pop);
    std::cout << "Number of morphologies to load: " << ids.size() << std::endl;
    std::vector<Morpho*> morphologies = circuit.getNeurons(ids, &_limits);
    uint32_t size = morphologies.size();
    std::cout << "Number of morphologies loaded: " << size << std::endl;

    geometry::HierarchicalAABBs morphoAABBs(size);
    std::vector<geometry::Edges> edgesSet(size);
    std::vector<geometry::Nodes> nodesSet(size);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        edgesSet[i] = morphologies[i]->edges;
        resample(edgesSet[i], 0.15);
        removeOutEdges(edgesSet[i], _limits);
        nodesSet[i] = uniqueNodes(edgesSet[i]);
        morphoAABBs[i] = new geometry::HierarchicalAABB(edgesSet[i]);
    }

    _setMeshes(edgesSet);

    uint32_t totalIters = 0;

    uint32_t cols =
        _solveCollisions(morphoAABBs, edgesSet, nodesSet, _limits, totalIters);

    _setCameraPos(_limits);
}

uint32_t OverlapCircuitApp::_solveCollisions(
    geometry::HierarchicalAABBs& aabbs,
    std::vector<geometry::Edges>& edgesSet,
    std::vector<geometry::Nodes>& nodesSet,
    geometry::AxisAlignedBoundingBox& limits,
    uint32_t& totalIters)
{
    geometry::Edges edges;
    for (uint32_t i = 0; i < edgesSet.size(); ++i)
        edges.insert(edges.end(), edgesSet[i].begin(), edgesSet[i].end());
    geometry::Nodes nodes = geometry::uniqueNodes(edges);

    std::cout << "Simulation with " << edges.size() << " springs and "
              << nodes.size() << " nodes" << std::endl;

    auto startTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime;
    uint32_t collisions = 1;
    uint32_t numIters = 1000;
    float ks = _ks;

    while (collisions > 0)
    {
        for (uint32_t iter = 0; iter < numIters; ++iter)
        {
            collisions = _solver->anim(aabbs, edgesSet, nodesSet, limits, ks,
                                       _ksc, 0.0, _threshold);

            if (totalIters % 100 == 0)
            {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
                for (uint32_t i = 0; i < edgesSet.size(); ++i)
                {
                    graphics::updateGeometry(_scene->meshes[i], edgesSet[i],
                                             _palette->color(i) * 0.5f,
                                             _palette->color(i),
                                             _palette->color(i) * 0.2f);
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
        if (ks < _ksLimit) break;
    }

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < edgesSet.size(); ++i)
    {
        graphics::updateGeometry(_scene->meshes[i], edgesSet[i],
                                 _palette->color(i) * 0.5f, _palette->color(i),
                                 _palette->color(i) * 0.2f);
    }

    elapsedTime = std::chrono::steady_clock::now() - startTime;
    std::cout << "Final result -> Iter: " << totalIters
              << "  Collisions: " << collisions << "  Stiffness: " << ks
              << "  Time: " << elapsedTime.count() << " seconds." << std::endl;

    return collisions;
};

void OverlapCircuitApp::_setMeshes(std::vector<geometry::Edges>& edgesSet)
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
        _scene->meshes[i] = graphics::generateMesh(
            edgesSet[i], _palette->color(i) * 0.5f, _palette->color(i),
            _palette->color(i) * 0.2f);
    }
}

void OverlapCircuitApp::_mouseButtonCallback(GLFWwindow* window,
                                             int button,
                                             int action,
                                             int mods)
{
    if (_scene)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        _mouseX = mouseX;
        _mouseY = mouseY;

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
