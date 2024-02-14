#include <phyanim/Phyanim.h>

#include <chrono>
#include <iomanip>
#include <iostream>

using namespace phyanim;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage error:\nUse: " << argv[0]
                  << " [-tet] [-off] file_name" << std::endl;
        return 0;
    }

    std::string ext(".off");
    std::vector<std::string> files;

    for (uint32_t i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);
        if (option.compare("-tet") == 0)
        {
            ext = std::string(".tet");
        }
        else if (option.compare("-off") == 0)
        {
            ext = std::string(".off");
        }
        else
        {
            files.push_back(option);
        }
    }

    std::cout << std::fixed << std::setprecision(2);
    float progress = 0.0;
    std::cout << "\rConverting files " << progress << "%" << std::flush;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        size_t extPos = 0;
        geometry::MeshPtr mesh = nullptr;
        std::string baseFile;
        if ((extPos = files[i].find(".node")) != std::string::npos)
        {
            if ((i + 1) < files.size() &&
                files[i + 1].find(".ele") != std::string::npos)
            {
                mesh = new geometry::Mesh();
                mesh->load(files[i], files[i + 1]);
                baseFile = files[i].substr(0, extPos);
                ++i;
            }
        }
        else if ((extPos = files[i].find(".off")) != std::string::npos)
        {
            mesh = new geometry::Mesh();
            mesh->load(files[i]);
            baseFile = files[i].substr(0, extPos);
        }
        else if ((extPos = files[i].find(".ply")) != std::string::npos)
        {
            mesh = new geometry::Mesh();
            mesh->load(files[i]);
            baseFile = files[i].substr(0, extPos);
        }
        else if ((extPos = files[i].find(".obj")) != std::string::npos)
        {
            mesh = new geometry::Mesh();
            mesh->load(files[i]);
            baseFile = files[i].substr(0, extPos);
        }
        else if ((extPos = files[i].find(".tet")) != std::string::npos)
        {
            mesh = new geometry::Mesh();
            mesh->load(files[i]);
            baseFile = files[i].substr(0, extPos);
        }

        if (mesh)
        {
            if ((extPos = baseFile.find_last_of('/')) != std::string::npos)
                baseFile = baseFile.substr(extPos + 1);
            baseFile.append(ext);
            mesh->write(baseFile);
            delete mesh;
        }
#pragma omp critical
        {
            progress += 100.0f / files.size();
            std::cout << "\rConverting files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
}