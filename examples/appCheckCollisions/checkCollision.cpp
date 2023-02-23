#include <Phyanim.h>

#include <chrono>
#include <iomanip>
#include <iostream>

phyanim::Meshes loadMeshes(std::vector<std::string> files)
{
    phyanim::Meshes meshes(files.size());
    double progress = 0.0;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\rLoading files " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        phyanim::MeshPtr mesh = new phyanim::Mesh(50.0, 1.0, 1.0, 0.3);
        mesh->load(files[i]);
        mesh->boundingBox =
            new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
        meshes[i] = mesh;
#pragma omp critical
        {
            progress += 100.0f / files.size();
            std::cout << "\rLoading files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Files loaded in: " << elapsedTime.count() << " seconds"
              << std::endl;
    return meshes;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage error:\nUse: " << argv[0] << " file_name file_name"
                  << std::endl;
        return 0;
    }

    std::vector<std::string> files;
    for (uint32_t i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);
        if (option.find(".tet") != std::string::npos)
            files.push_back(option);
        else if (option.find(".off") != std::string::npos)
            files.push_back(option);
    }

    auto meshes = loadMeshes(files);

    auto aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(meshes, 15);
    std::cout << "Number of collisions: " << aabbs.size() << std::endl;

    return 0;
}
