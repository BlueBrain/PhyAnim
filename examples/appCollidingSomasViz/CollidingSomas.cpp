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
    _palette = new ColorPalette(CONTRAST);
}

void CollidingSomas::_actionLoop()
{
    RadiusFunc radiusFunc = RadiusFunc::MIN_NEURITES;

    _limits.lowerLimit(phyanim::Vec3(-1000, -1000, -1000));
    _limits.upperLimit(phyanim::Vec3(1000, 1000, 1000));

    std::string circuitPath;
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
    _solver = new CollisionSolver(0.001);

    _setCameraPos(_limits);
    Circuit circuit(circuitPath, pop);
    std::vector<Morpho*> morphologies =
        circuit.getNeurons(ids, nullptr, radiusFunc, false);

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
    std::vector<phyanim::Nodes> nodesSet(size);
    phyanim::HierarchicalAABBs aabbs(size);
    // Check soma collisions

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        auto soma0 = morphologies[i]->soma;
        for (auto node : morphologies[i]->sectionNodes)
        {
            auto nodeCenter = new phyanim::Node(
                soma0->position, 0, node->radius, phyanim::Vec3::Zero(),
                phyanim::Vec3::Zero(), node->radius);
            edgesSet[i].push_back(new phyanim::Edge(nodeCenter, node));
        }
        edgesSet[i].push_back(new phyanim::Edge(soma0, soma0));
        nodesSet[i] = phyanim::uniqueNodes(edgesSet[i]);
        aabbs[i] = new phyanim::HierarchicalAABB(edgesSet[i]);
    }

    _setMeshes(edgesSet);

    _solveCollisions(aabbs, edgesSet, nodesSet);
}

uint32_t CollidingSomas::_solveCollisions(phyanim::HierarchicalAABBs& aabbs,
                                          std::vector<phyanim::Edges>& edgesSet,
                                          std::vector<phyanim::Nodes>& nodesSet)
{
    uint32_t numCollisions = 1;
    uint32_t iter = 0;
    double ksc = 1000.0;

    auto startTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime;

    while (numCollisions > 0)
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < nodesSet.size(); ++i)
        {
            phyanim::clearCollision(nodesSet[i]);
            phyanim::clearForce(nodesSet[i]);
        }
        numCollisions =
            phyanim::CollisionDetection::computeCollisions(aabbs, ksc);

        _solver->animSomas(aabbs, nodesSet);

        if (iter % 100 == 0 | numCollisions == 0)
        {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
            for (uint32_t i = 0; i < aabbs.size(); ++i)
            {
                updateGeometry(_scene->meshes[i], edgesSet[i],
                               _palette->color(i) * 0.3, _palette->color(i),
                               _palette->color(i) * 0.2);
            }
            elapsedTime = std::chrono::steady_clock::now() - startTime;
            std::cout << "Iter: " << iter << "  Collisions: " << numCollisions
                      << "  Time: " << elapsedTime.count() << " seconds."
                      << std::endl;
        }

        ++iter;
    }
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < aabbs.size(); ++i)
    {
        updateGeometry(_scene->meshes[i], edgesSet[i], _palette->color(i) * 0.3,
                       _palette->color(i), _palette->color(i) * 0.2);
    }

    return iter;
}

void CollidingSomas::_setMeshes(std::vector<phyanim::Edges>& edgesSet)
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
            generateMesh(edgesSet[i], _palette->color(i) * 0.3,
                         _palette->color(i), _palette->color(i) * 0.2);
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
