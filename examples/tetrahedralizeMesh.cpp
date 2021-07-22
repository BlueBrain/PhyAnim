#include <iostream>
// #include <cstdio>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/readOBJ.h>
#include <igl/readOFF.h>
#include <igl/readPLY.h>
#include <igl/writeOFF.h>

#include <fstream>

int main(int argc, char* argv[])
{
    std::string outFile("out_file.tet");
    std::string inFile;
    for (int i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);
        if (option.compare("-out") == 0)
        {
            outFile = std::string(argv[i + 1]);
            ++i;
        }
        else
        {
            inFile = std::string(argv[i]);
        }
    }

    if (inFile.empty())
    {
        std::cerr << "Error Usage: " << argv[0]
                  << " mesh[.off|.ply|.obj] [-out out_file]" << std::endl;
        return 0;
    }

    Eigen::MatrixXd sVertices;
    Eigen::MatrixXi sFacets;

    Eigen::MatrixXd vertices;
    Eigen::MatrixXi tets;
    Eigen::MatrixXi facets;

    if (inFile.find(".off") != std::string::npos)
    {
        igl::readOFF(inFile.c_str(), sVertices, sFacets);
    }
    else if (inFile.find(".ply") != std::string::npos)
    {
        igl::readPLY(inFile.c_str(), sVertices, sFacets);
    }
    else if (inFile.find(".obj") != std::string::npos)
    {
        igl::readOBJ(inFile.c_str(), sVertices, sFacets);
    }
    else
    {
        return 0;
    }

    igl::copyleft::tetgen::tetrahedralize(sVertices, sFacets, "a0.01", vertices,
                                          tets, facets);

    std::ofstream os(outFile.c_str());
    if (!os.is_open())
    {
        return 0;
    }

    os << "tet " << vertices.rows() << " " << tets.rows() << "\n";
    os << vertices.format(Eigen::IOFormat(Eigen::FullPrecision,
                                          Eigen::DontAlignCols, " ", "\n", "",
                                          "", "", "\n"));
    for (size_t i = 0; i < tets.rows(); i++)
    {
        os << tets(i, 0) << " " << tets(i, 1) << " " << tets(i, 2) << " "
           << tets(i, 3) << "\n";
    }

    std::cout << "Mesh tetrahedalized with: " << vertices.rows() << " nodes, "
              << tets.rows() << " tetrahedra." << std::endl;
    return 1;
}
