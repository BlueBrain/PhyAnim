#include <Phyanim.h>

#include <chrono>
#include <iomanip>

double stiffness = 50.0;
double dt = 0.01;
double bbFactor = 15;
double initCollisionStiffness = 2.0;
double collisionStiffnessMultiplier = 0.1;

phyanim::Meshes meshes;
std::vector<phyanim::HierarchicalAABBPtr> tetAABBs;
phyanim::AnimSystem* animSys;

void setSurfaceNodes(phyanim::MeshPtr mesh)
{
    for (auto triangle : mesh->surfaceTriangles)
    {
        for (auto node : triangle->nodes())
        {
            node->surface = true;
        }
    }
}

void loadMeshes(std::vector<std::string> files)
{
    meshes.resize(files.size());
    tetAABBs.resize(files.size());
    double progress = 0.0;
    std::cout << "\rLoading files " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        auto mesh = new phyanim::Mesh(stiffness, 1.0, 1.0, 0.3);
        mesh->load(files[i]);
        mesh->boundingBox =
            new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
        meshes[i] = mesh;
        tetAABBs[i] = new phyanim::HierarchicalAABB(mesh->tetrahedra);
        setSurfaceNodes(mesh);
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
}

void writeMeshes(phyanim::Meshes meshes,
                 std::vector<std::string> files,
                 std::string extension)
{
    double progress = 0.0;
    std::cout << "\rSaving files " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < meshes.size(); ++i)
    {
        auto outFile = files[i];
        uint32_t pos = outFile.find_last_of('/');
        if (pos != std::string::npos) outFile = outFile.substr(pos + 1);
        pos = outFile.find(".tet");
        if (pos != std::string::npos) outFile = outFile.substr(0, pos);
        outFile += extension + ".tet";
        meshes[i]->write(outFile);
#pragma omp critical
        {
            progress += 100.0f / meshes.size();
            std::cout << "\rSaving files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Files saved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

phyanim::MeshPtr sliceMesh(phyanim::HierarchicalAABBPtr tetAABB,
                           const phyanim::AxisAlignedBoundingBox& aabb)
{
    phyanim::MeshPtr sliceMesh = nullptr;
    auto tets = tetAABB->insidePrimitives(aabb);
    if (tets.size() > 0)
    {
        sliceMesh = new phyanim::Mesh(stiffness, 1.0, 1.0, 0.3);
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

void sortAABBs(phyanim::AxisAlignedBoundingBoxes& aabbs)
{
    auto cmp = [](phyanim::AxisAlignedBoundingBoxPtr a,
                  phyanim::AxisAlignedBoundingBoxPtr b) {
        return a->radius() > b->radius();
    };
    std::sort(aabbs.begin(), aabbs.end(), cmp);
}

void resolveCollision(phyanim::AxisAlignedBoundingBoxPtr aabb)
{
    auto startTime = std::chrono::steady_clock::now();
    phyanim::Meshes slicedMeshes;
    for (uint32_t j = 0; j < meshes.size(); ++j)
    {
        auto slicedMesh = sliceMesh(tetAABBs[j], *aabb);
        if (slicedMesh)
        {
            slicedMeshes.push_back(slicedMesh);
        }
    }
    animSys->preprocessMesh(slicedMeshes);
    double collisionStiffness = initCollisionStiffness;

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
            collisionStiffness +=
                initCollisionStiffness * collisionStiffnessMultiplier;
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
#pragma omp critical
    {
        std::cout << "Collision with radius: " << aabb->radius()
                  << "\tsolved in: " << elapsedTime.count() << " seconds"
                  << std::endl;
    }
}

void resolveCollisions(phyanim::AxisAlignedBoundingBoxes& aabbs)
{
    double progress = 0.0;
    // std::cout << "\rSolving collisions " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

    uint32_t id = 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < aabbs.size(); ++i)
    {
        uint32_t cId = i;

#ifdef PHYANIM_USES_OPENMP
#pragma omp critical
        {
            cId = id;
            ++id;
        }
#endif
        auto aabb = aabbs[cId];
        resolveCollision(aabb);
#pragma omp critical
        {
            progress += 100.0f / aabbs.size();
            // std::cout << "\rSolving collisions " << progress << "%"
            //           << std::endl;
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Overlap solved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> files;
    for (uint32_t i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);

        if (option.compare("-dt") == 0)
        {
            ++i;
            dt = std::atof(argv[i]);
        }
        else if (option.compare("-k") == 0)
        {
            ++i;
            stiffness = std::atof(argv[i]);
        }
        else if (option.find(".tet") != std::string::npos)
            files.push_back(option);
    }

    animSys = new phyanim::ImplicitFEMSystem(dt);
    animSys->gravity = false;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Overlap run with dt: " << dt << " stiffness: " << stiffness
              << std::endl;

    // Loading files
    loadMeshes(files);

    // Collisions resolution
    auto aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(meshes, bbFactor);
    std::cout << "Number of collisions: " << aabbs.size() << std::endl;
    sortAABBs(aabbs);
    resolveCollisions(aabbs);

    // Writing files
    writeMeshes(meshes, files, "_no_overlap");
}