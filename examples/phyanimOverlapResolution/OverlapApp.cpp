#include "OverlapApp.h"

#include <ImplicitFEMSystem.h>

#include <iostream>

int main(int argc, char* argv[])
{
    examples::OverlapApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
OverlapApp::OverlapApp(int argc, char** argv) : GLFWApp(argc, argv) {}

void OverlapApp::_actionLoop()
{
    std::string usage(
        "SYNOPSIS\n\t"
        " mesh_file[.tet|.node .ele] [-dt double] [-d double] "
        "[-ks double] [-kd double] [-kp double] "
        "[--help]"
        "\nThe following options are available:"
        "\n\t-dt double    Set time increment"
        "\n\t-d double     Set density"
        "\n\t-ks double    Set stiffness constant/Young modulus"
        "\n\t-kp double    Set poisson ratio"
        "\n\t--help        Print help message");
    if (_args.size() < 1)
    {
        std::cerr << "Usage error:\n" << usage << std::endl;
        exit(-1);
    }

    double stiffness = 1000.0;
    double density = 1.0;
    double damping = 1.0;
    double poissonRatio = 0.3;
    double collisionStiffness = 100.0;
    double dt = 0.01;
    bool stepByStep = true;

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
            else if (option.compare("-kp") == 0)
            {
                poissonRatio = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kd") == 0)
            {
                damping = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kc") == 0)
            {
                collisionStiffness = std::stof(_args[i + 1]);
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

    std::cout << "stiffness: " << stiffness
              << "\ncollision stiffness: " << collisionStiffness
              << "\npoisson ratio: " << poissonRatio << "\ndt: " << dt
              << std::endl;
    auto animSys = new phyanim::ImplicitFEMSystem(dt);
    animSys->gravity = false;

    if (!stepByStep)
    {
        _setAnim(true);
    }

    auto startTime = std::chrono::steady_clock::now();

    phyanim::Meshes meshes;
    phyanim::AxisAlignedBoundingBox limits;

    for (uint32_t i = 0; i < files.size(); ++i)
    {
        phyanim::DrawableMesh* mesh = nullptr;
        std::string file = files[i];

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
            mesh = new phyanim::DrawableMesh(stiffness, density, 0.1,
                                             poissonRatio);
            mesh->load(file);
        }

        if (mesh)
        {
            std::string outFile = file.substr(0, extPos) + "_sol.tet";
            mesh->compute();
            mesh->boundingBox =
                new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
            limits.unite(*mesh->boundingBox);
            _setCameraPos(*mesh->boundingBox);
            _scene->addMesh(mesh);

            phyanim::Meshes dynamics(1);
            dynamics[0] = mesh;

            bool collision = true;
            while (collision)
            {
                if (_getAnim())
                {
                    mesh->nodesForceZero();

                    collision = phyanim::CollisionDetection::computeCollisions(
                        dynamics, meshes, collisionStiffness, true);
                    if (collision)
                    {
                        if (mesh->kMatrix.data().size() == 0)
                            animSys->preprocessMesh(mesh);

                        animSys->step(mesh);
                        mesh->boundingBox->update();
                        phyanim::CollisionDetection::computeCollisions(dynamics,
                                                                       limits);
                        mesh->updatedPositions = true;
                    }
                    else
                    {
                        mesh->write(outFile);
                        std::cout << "\t* Saved file " << outFile << std::endl;

                        meshes.push_back(mesh);
                        if (stepByStep)
                        {
                            _setAnim(false);
                        }
                    }
                    mesh->updateColors();
                }
            }
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Overlap solved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

}  // namespace examples
