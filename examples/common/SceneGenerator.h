#ifndef __EXAMPLES_SCENE_GENERATOR__
#define __EXAMPLES_SCENE_GENERATOR__

#include <Phyanim.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

namespace examples
{
class SceneGenerator
{
public:
    static phyanim::Meshes generate(phyanim::Meshes meshes,
                                    uint32_t numOutMeshes,
                                    phyanim::AxisAlignedBoundingBox aabb)
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

            std::cout << "Genrating process 0.00%" << std::flush;
            for (uint32_t i = 0; i < numOutMeshes; ++i)
            {
                uint32_t meshId = std::rand() % numMeshes;
                auto mesh = meshes[meshId]->copy();

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
                sceneMeshes.push_back(mesh);

                std::cout << "\rGenrating process "
                          << (i + 1) * 100.0 / numOutMeshes << "%"
                          << std::flush;
            }
            std::cout << std::endl;
        }
        return sceneMeshes;
    };

private:
    SceneGenerator(){};
};
}  // namespace examples

#endif