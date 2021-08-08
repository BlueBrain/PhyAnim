#include <CollisionDetection.h>

#include <chrono>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage error:\nUse: " << argv[0] << " file_name file_name"
                  << std::endl;
        return 0;
    }

    phyanim::Meshes meshes(2);
    meshes[0] = new phyanim::Mesh();
    meshes[1] = new phyanim::Mesh();
    meshes[0]->load(std::string(argv[1]));
    meshes[1]->load(std::string(argv[2]));

    uint64_t iterations = 100;
    uint64_t cellSizeInit = 1;
    uint64_t cellSizeEnd = 10;

    if (argc > 3) iterations = std::atoi(argv[3]);
    if (argc > 4) cellSizeInit = std::atoi(argv[4]);
    if (argc > 5) cellSizeEnd = std::atoi(argv[5]);

    meshes[0]->boundingBox =
        new phyanim::HierarchicalAABB(meshes[0]->surfaceTriangles);
    meshes[1]->boundingBox =
        new phyanim::HierarchicalAABB(meshes[1]->surfaceTriangles);

    std::chrono::time_point<std::chrono::steady_clock> startTime =
        std::chrono::steady_clock::now();
    auto bbs = phyanim::CollisionDetection::collisionBoundingBoxes(meshes, 2);

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;

    std::cout << "Number of collision bounding boxes: " << bbs.size()
              << "\ttime: " << elapsedTime.count() * 1000 << " ms" << std::endl;

    std::cout << "\nIterations: " << iterations << std::endl;

    for (uint64_t cellSize = cellSizeInit; cellSize <= cellSizeEnd; ++cellSize)
    {
        delete meshes[0]->boundingBox;
        delete meshes[1]->boundingBox;
        meshes[0]->boundingBox = new phyanim::HierarchicalAABB(
            meshes[0]->surfaceTriangles, cellSize);

        meshes[1]->boundingBox = new phyanim::HierarchicalAABB(
            meshes[1]->surfaceTriangles, cellSize);

        startTime = std::chrono::steady_clock::now();

        for (uint64_t i = 0; i < iterations; ++i)
            phyanim::CollisionDetection::computeCollisions(meshes);
        endTime = std::chrono::steady_clock::now();
        elapsedTime = endTime - startTime;
        std::cout << "Cell size: " << cellSize
                  << "\ttime: " << elapsedTime.count() / iterations * 1000
                  << " ms" << std::endl;
    }
}
