#include "AnimMeshApp.h"

#include <ExplicitFEMSystem.h>
#include <ExplicitMassSpringSystem.h>
#include <ImplicitFEMSystem.h>
#include <ImplicitMassSpringSystem.h>

#include <iostream>

int main(int argc, char* argv[])
{
    examples::AnimMeshApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
void AnimMeshApp::_actionLoop()
{
    double stiffness = 1000.0;
    double poisson = 0.3;
    double collisionStiffness = 200.0;
    double dt = 0.01;
    double damping = 1.0;
    double density = 1.0;
    phyanim::AnimSystem* animSys = new phyanim::ImplicitFEMSystem(dt);

    phyanim::Meshes meshes;

    phyanim::AxisAlignedBoundingBox limits;

    for (uint64_t i = 0; i < _args.size(); ++i)
    {
        auto mesh =
            new phyanim::DrawableMesh(stiffness, density, damping, poisson);
        std::string file = _args[i];

        if (file.find(".node") != std::string::npos)
        {
            ++i;
            std::string file1 = _args[i];
            mesh->load(file, file1);
        }
        else if (file.find(".tet") != std::string::npos)
            mesh->load(file);
        else
            std::cerr << "Error: Unknown format file." << std::endl;

        mesh->compute();

        meshes.push_back(mesh);
        mesh->boundingBox =
            new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
        mesh->updateColor();

        limits.unite(*mesh->boundingBox);
        _setCameraPos(limits);

        _scene->addMesh(mesh);
    }

    animSys->preprocessMesh(meshes);

    phyanim::Vec3 halfSides = limits.upperLimit() - limits.center();
    limits.lowerLimit(limits.lowerLimit() - halfSides);
    limits.upperLimit(limits.upperLimit() + halfSides);

    while (true)
    {
        if (_getAnim())
        {
            for (auto mesh : meshes) mesh->nodesForceZero();

            phyanim::CollisionDetection::computeCollisions(
                meshes, collisionStiffness, true);

            animSys->step(meshes);

            for (auto mesh : meshes) mesh->boundingBox->update();

            phyanim::CollisionDetection::computeCollisions(meshes, limits);

            for (auto mesh : meshes)
            {
                auto drawMesh = dynamic_cast<phyanim::DrawableMesh*>(mesh);
                drawMesh->updatedPositions = true;
                drawMesh->updateColors();
            }
        }
    }
}

}  // namespace examples
