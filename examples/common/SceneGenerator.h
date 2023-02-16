#ifndef __EXAMPLES_SCENE_GENERATOR__
#define __EXAMPLES_SCENE_GENERATOR__

#include <Phyanim.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace examples
{
class SceneGenerator
{
public:
    static phyanim::Meshes generate(phyanim::Meshes meshes,
                                    uint32_t numOutMeshes,
                                    phyanim::AxisAlignedBoundingBox aabb,
                                    double bbFactor = 15.0,
                                    uint32_t maxCollisions = 4,
                                    double maxCollisionRadius = 4.0)
    {
        std::srand(std::time(nullptr));
        phyanim::Meshes sceneMeshes;
        uint32_t numMeshes = meshes.size();

        if (numMeshes > 0)
        {
            phyanim::Vec3 range(aabb.upperLimit() - aabb.lowerLimit());

            std::cout << std::fixed << std::setprecision(2);
            double progress = 0.0;
            std::cout << "Generating process " << progress << "%" << std::flush;

            uint32_t numCollisions = 0;
            for (uint32_t i = 0; i < numOutMeshes; ++i)
            {
                uint32_t meshId = std::rand() % numMeshes;
                auto mesh = meshes[meshId]->copy();
                sceneMeshes.push_back(mesh);
                mesh->boundingBox =
                    new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
                bool rightPos = false;

                while (!rightPos)
                {
                    phyanim::Vec3 pos;
                    pos.x() = (double)std::rand() / RAND_MAX * range.x();
                    pos.y() = (double)std::rand() / RAND_MAX * range.y();
                    pos.z() = (double)std::rand() / RAND_MAX * range.z();

                    double alpha = (double)std::rand() / RAND_MAX * 2 * M_PI;
                    double beta = (double)std::rand() / RAND_MAX * 2 * M_PI;
                    double gamma = (double)std::rand() / RAND_MAX * 2 * M_PI;

                    double cosX = std::cos(alpha);
                    double sinX = std::sin(alpha);
                    double cosY = std::cos(beta);
                    double sinY = std::sin(beta);
                    double cosZ = std::cos(gamma);
                    double sinZ = std::sin(gamma);
                    phyanim::Mat3 rot;
                    phyanim::Mat3 xRot;
                    phyanim::Mat3 yRot;
                    phyanim::Mat3 zRot;
                    zRot << cosZ, -sinZ, 0, sinZ, cosZ, 0, 0, 0, 1;
                    yRot << cosY, 0, sinY, 0, 1, 0, -sinY, 0, cosY;
                    xRot << 1, 0, 0, 0, cosX, -sinX, 0, sinX, cosX;
                    rot = xRot * yRot * zRot;

                    phyanim::Vec3 center = mesh->boundingBox->center();
                    for (auto node : mesh->nodes)
                    {
                        phyanim::Vec3 position = node->position - center;
                        position = rot * position;
                        position += pos;
                        node->initPosition = position;
                        node->position = position;
                        node->normal = rot * node->normal;
                    }
                    mesh->boundingBox->update();

                    auto aabbs =
                        phyanim::CollisionDetection::collisionBoundingBoxes(
                            sceneMeshes, bbFactor);

                    rightPos =
                        ((aabbs.size() - numCollisions) <= maxCollisions);
                    if (rightPos)
                    {
                        for (auto aabb : aabbs)
                            rightPos &= aabb->radius() < maxCollisionRadius;
                    }

                    if (rightPos) numCollisions = aabbs.size();
                }
                progress += 100.0 / numOutMeshes;
                std::cout << "\rGenerating process " << progress << "%"
                          << std::flush;
            }
            std::cout << std::endl;
            std::cout << "Number of collisions: " << numCollisions << std::endl;
        }
        return sceneMeshes;
    };

private:
    SceneGenerator(){};
};
}  // namespace examples

#endif