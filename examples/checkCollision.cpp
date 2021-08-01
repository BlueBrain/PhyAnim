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
    phyanim::Mesh* mesh0 = new phyanim::Mesh();
    phyanim::Mesh* mesh1 = new phyanim::Mesh();
    mesh0->load(std::string(argv[1]));
    mesh1->load(std::string(argv[2]));

    uint64_t iterations = 100;
    uint64_t cellSizeInit = 5;
    uint64_t cellSizeEnd = 10;

    if (argc > 3) iterations = std::atoi(argv[3]);
    if (argc > 4) cellSizeInit = std::atoi(argv[4]);
    if (argc > 5) cellSizeEnd = std::atoi(argv[5]);

    std::cout << "\nIterations: " << iterations << std::endl;

    for (uint64_t cellSize = cellSizeInit; cellSize <= cellSizeEnd; ++cellSize)
    {
        phyanim::HierarchicalAABBs dynamics;
        dynamics.push_back(
            new phyanim::HierarchicalAABB(mesh0->surfaceTriangles, cellSize));
        dynamics.push_back(
            new phyanim::HierarchicalAABB(mesh1->surfaceTriangles, cellSize));

        std::chrono::time_point<std::chrono::steady_clock> startTime =
            std::chrono::steady_clock::now();

        for (uint64_t i = 0; i < iterations; ++i)
            phyanim::CollisionDetection::computeCollisions(dynamics);
        auto endTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = endTime - startTime;
        std::cout << "Cell size: " << cellSize
                  << "\ttime: " << elapsedTime.count() / iterations * 1000
                  << " ms" << std::endl;
    }
}
