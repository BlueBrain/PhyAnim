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
                                    uint32_t maxCollisions = 4)
    {
        std::srand(std::time(nullptr));
        phyanim::Meshes sceneMeshes;
        uint32_t numMeshes = meshes.size();

        if (numMeshes > 0)
        {
            std::vector<phyanim::Vec3> centers;
            for (auto mesh : meshes)
            {
                phyanim::AxisAlignedBoundingBox aabb(mesh->surfaceTriangles);
                centers.push_back(aabb.center());
            }

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
                    pos += aabb.lowerLimit();
                    pos -= centers[meshId];

                    for (auto node : mesh->nodes)
                    {
                        node->position += pos;
                        node->initPosition += pos;
                    }
                    mesh->boundingBox->update();

                    auto aabbs =
                        phyanim::CollisionDetection::collisionBoundingBoxes(
                            sceneMeshes, 1.0001);

                    rightPos =
                        ((aabbs.size() - numCollisions) <= maxCollisions);
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