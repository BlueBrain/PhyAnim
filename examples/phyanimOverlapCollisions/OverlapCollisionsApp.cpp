#include "OverlapCollisionsApp.h"

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    examples::OverlapCollisionsApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
OverlapCollisionsApp::OverlapCollisionsApp(int argc, char** argv)
    : GLFWApp(argc, argv)
{
}

void OverlapCollisionsApp::_actionLoop()
{
    double stiffness = 50.0;
    double damping = 1.0;
    double density = 1.0;
    double poissonRatio = 0.3;
    double initCollisionStiffness = 2.0;
    double collisionStiffness = 2.0;
    double collisionStiffnessMultiplier = 0.1;
    double dt = 0.01;
    double bbFactor = 15;
    bool stepByStep = true;

    std::chrono::time_point<std::chrono::steady_clock> startTime;

    std::string usage = std::string(
        "SYNOPSIS\n\t"
        " mesh_file[.tet|.node .ele] [-dt double] [-d double] "
        "[-ks double] [-kd double] [-kp double] "
        "[--help]"
        "\nThe following options are available:"
        "\n\t-dt double    Set time increment"
        "\n\t-d double     Set density"
        "\n\t-ks double    Set stiffness constant/Young modulus"
        "\n\t-kd double    Set damping constant"
        "\n\t-kp double    Set poisson ratio"
        "\n\t--help        Print help message");
    if (_args.size() == 0)
    {
        std::cerr << "Usage error:\n" << usage << std::endl;
        exit(-1);
    }

    std::vector<std::string> files;
    for (int i = 0; i < _args.size(); ++i)
    {
        std::string option(_args[i]);
        try
        {
            if (option.compare("-dt") == 0)
            {
                dt = std::stof(_args[i + 1]);
                ++i;
            }
            if (option.compare("-bb") == 0)
            {
                bbFactor = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-ks") == 0)
            {
                stiffness = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-d") == 0)
            {
                density = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kd") == 0)
            {
                damping = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kp") == 0)
            {
                poissonRatio = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kc") == 0)
            {
                collisionStiffness = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kcm") == 0)
            {
                collisionStiffnessMultiplier = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-cont") == 0)
            {
                stepByStep = false;
            }
            else if (option.compare("--help") == 0)
            {
                std::cout << usage << std::endl;
                exit(0);
            }
            else
            {
                files.push_back(std::string(_args[i]));
            }
        }
        catch (...)
        {
            std::cerr << "Usage error:\n" << usage << std::endl;
            exit(-1);
        }
    }

    initCollisionStiffness = collisionStiffness;

    phyanim::AnimSystem* animSys = new phyanim::ImplicitFEMSystem(dt);
    animSys->gravity = false;

    phyanim::Meshes meshes;
    std::vector<std::string> outFiles;

    phyanim::AxisAlignedBoundingBox limits;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Loading files 0.00%" << std::flush;

    startTime = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        phyanim::DrawableMesh* mesh = nullptr;
        std::string file = files[i];

        std::string outFile;
        size_t extPos = 0;
        if ((extPos = file.find(".node")) != std::string::npos)
        {
            std::string file1 = files[i + 1];
            ++i;
            mesh = new phyanim::DrawableMesh(stiffness, density, damping,
                                             poissonRatio);
            mesh->load(file, file1);
        }
        else if ((extPos = file.find(".tet")) != std::string::npos)
        {
            mesh = new phyanim::DrawableMesh(stiffness, density, damping,
                                             poissonRatio);
            mesh->load(file);
        }

        if (mesh)
        {
            std::string outFile = file.substr(0, extPos) + "_sol.tet";
            outFiles.push_back(outFile);
            meshes.push_back(mesh);
            // mesh->compute();
            _setSurfaceNodes(mesh);
            mesh->boundingBox =
                new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
            limits.unite(*mesh->boundingBox);
            _scene->addMesh(mesh);
            _setCameraPos(limits);
        }
        std::cout << "\rLoading files " << (i + 1) * 100.0 / _args.size() << "%"
                  << std::flush;
    }
    std::cout << "\rLoading files 100.00%" << std::endl;

    _setCameraPos(limits);

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Files loaded in: " << elapsedTime.count() << " seconds"
              << std::endl;

    startTime = endTime;

    phyanim::AxisAlignedBoundingBoxes aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(meshes, bbFactor);
    std::cout << "Number of collisions: " << aabbs.size() << std::endl;
    uint32_t collisionId = 0;

    if (stepByStep)
    {
        for (uint32_t i = 0; i < aabbs.size(); ++i)
        {
            auto aabb = aabbs[i];
            std::cout << "Collision id: " << collisionId << std::endl;
            ++collisionId;
            _setCameraPos(*aabb, false);

            phyanim::Meshes slicedMeshes;
            phyanim::Meshes completeMeshes;
            for (auto mesh : meshes)
            {
                auto slicedMesh = _sliceMesh(mesh, *aabb, stiffness, density,
                                             damping, poissonRatio);
                if (slicedMesh)
                {
                    slicedMeshes.push_back(slicedMesh);
                    completeMeshes.push_back(mesh);
                }
            }

            animSys->preprocessMesh(slicedMeshes);

            _setAnim(!stepByStep);

            collisionStiffness = initCollisionStiffness;

            bool collision = phyanim::CollisionDetection::computeCollisions(
                slicedMeshes, collisionStiffness, true);
            for (auto mesh : completeMeshes)
                dynamic_cast<phyanim::DrawableMesh*>(mesh)->updateColors();
            while (collision)
            {
                if (_getAnim())
                {
                    for (auto mesh : slicedMeshes) mesh->nodesForceZero();
                    collision = phyanim::CollisionDetection::computeCollisions(
                        slicedMeshes, collisionStiffness, true);
                    if (collision)
                    {
                        animSys->step(slicedMeshes);
                        for (auto mesh : slicedMeshes)
                            mesh->boundingBox->update();
                        phyanim::CollisionDetection::computeCollisions(
                            slicedMeshes, limits);
                        for (auto mesh : completeMeshes)
                            dynamic_cast<phyanim::DrawableMesh*>(mesh)
                                ->updatedPositions = true;
                        collisionStiffness += initCollisionStiffness *
                                              collisionStiffnessMultiplier;
                    }
                    for (auto mesh : completeMeshes)
                        dynamic_cast<phyanim::DrawableMesh*>(mesh)
                            ->updateColors();
                }
            }
        }
    }
    else
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < aabbs.size(); ++i)
        {
            auto aabb = aabbs[i];

            phyanim::Meshes slicedMeshes;
            phyanim::Meshes completeMeshes;
            for (auto mesh : meshes)
            {
                auto slicedMesh = _sliceMesh(mesh, *aabb, stiffness, density,
                                             damping, poissonRatio);
                if (slicedMesh)
                {
                    slicedMeshes.push_back(slicedMesh);
                    completeMeshes.push_back(mesh);
                }
            }

            animSys->preprocessMesh(slicedMeshes);
            collisionStiffness = initCollisionStiffness;

            bool collision = true;
            while (collision)
            {
                for (auto mesh : slicedMeshes) mesh->nodesForceZero();
                collision = phyanim::CollisionDetection::computeCollisions(
                    slicedMeshes, collisionStiffness, true);
                if (collision)
                {
                    animSys->step(slicedMeshes);
                    for (auto mesh : slicedMeshes) mesh->boundingBox->update();
                    phyanim::CollisionDetection::computeCollisions(slicedMeshes,
                                                                   limits);
                    for (auto mesh : completeMeshes)
                        dynamic_cast<phyanim::DrawableMesh*>(mesh)
                            ->updatedPositions = true;
                    collisionStiffness +=
                        initCollisionStiffness * collisionStiffnessMultiplier;
                }
                for (auto mesh : completeMeshes)
                    dynamic_cast<phyanim::DrawableMesh*>(mesh)->updateColors();
            }
        }
    }

    _setCameraPos(limits);
    endTime = std::chrono::steady_clock::now();
    elapsedTime = endTime - startTime;
    std::cout << "Overlap solved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

phyanim::MeshPtr OverlapCollisionsApp::_sliceMesh(
    phyanim::MeshPtr mesh,
    const phyanim::AxisAlignedBoundingBox& aabb,
    double stiffness,
    double density,
    double damping,
    double poissonRatio)
{
    phyanim::DrawableMesh* sliceMesh = nullptr;
    auto tetsAABB = new phyanim::HierarchicalAABB(mesh->tetrahedra);
    auto tets = tetsAABB->insidePrimitives(aabb);

    delete tetsAABB;

    if (tets.size() > 0)
    {
        sliceMesh = new phyanim::DrawableMesh(stiffness, density, damping,
                                              poissonRatio);
        sliceMesh->tetrahedra = tets;
        sliceMesh->tetsToNodes();
        sliceMesh->tetsToTriangles();
        sliceMesh->boundingBox =
            new phyanim::HierarchicalAABB(sliceMesh->surfaceTriangles);
        sliceMesh->compute();

        phyanim::UniqueTriangles uniqueTriangles;

        for (auto triangle : sliceMesh->surfaceTriangles)
        {
            bool surface = true;
            for (auto node : triangle->nodes())
            {
                surface &= node->surface;
            }
            if (!surface)
            {
                for (auto node : triangle->nodes())
                {
                    node->fix = true;
                }
            }
        }
    }
    return sliceMesh;
}

void OverlapCollisionsApp::_setSurfaceNodes(phyanim::MeshPtr mesh)
{
    for (auto triangle : mesh->surfaceTriangles)
    {
        for (auto node : triangle->nodes())
        {
            node->surface = true;
        }
    }
}

void OverlapCollisionsApp::_mouseButtonCallback(GLFWwindow* window,
                                                int button,
                                                int action,
                                                int mods)
{
    if (_scene)
    {
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

                glfwGetCursorPos(window, &_mouseX, &_mouseY);
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

                glfwGetCursorPos(window, &_mouseX, &_mouseY);
            }
            else if (action == GLFW_RELEASE)
            {
                _rightButtonPressed = false;
            }
        }
    }
}

}  // namespace examples
