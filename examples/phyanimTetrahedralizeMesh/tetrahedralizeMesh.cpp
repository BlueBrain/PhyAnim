#include <iostream>
// #include <cstdio>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/readOBJ.h>
#include <igl/readOFF.h>
#include <igl/readPLY.h>
#include <igl/writeOFF.h>

#include <fstream>

void tetrahedralizeMesh(const std::string& file)
{
    Eigen::MatrixXd sourceVertices;
    Eigen::MatrixXi sourceFacets;
// 
    Eigen::MatrixXd destVertices;
    Eigen::MatrixXi destTets;
    Eigen::MatrixXi destFacets;

    std::string outFile;
    size_t extPos;
    if ((extPos = file.find(".off")) != std::string::npos)
    {
        outFile = file.substr(0, extPos);
        igl::readOFF(file.c_str(), sourceVertices, sourceFacets);
    }
    else if ((extPos = file.find(".ply")) != std::string::npos)
    {
        outFile = file.substr(0, extPos);
        igl::readPLY(file.c_str(), sourceVertices, sourceFacets);
    }
    else if ((extPos = file.find(".obj")) != std::string::npos)
    {
        outFile = file.substr(0, extPos);
        igl::readOBJ(file.c_str(), sourceVertices, sourceFacets);
    }
    else
    {
        return;
    }
    outFile.append(".tet");

    igl::copyleft::tetgen::tetrahedralize(sourceVertices, sourceFacets,
                                          "Qa0.01", destVertices, destTets,
                                          destFacets);

    std::ofstream os(outFile.c_str());
    if (!os.is_open())
    {
        return;
    }

    os << "tet " << destVertices.rows() << " " << destTets.rows() << "\n";
    os << destVertices.format(Eigen::IOFormat(Eigen::FullPrecision,
                                              Eigen::DontAlignCols, " ", "\n",
                                              "", "", "", "\n"));
    for (size_t i = 0; i < destTets.rows(); i++)
    {
        os << destTets(i, 0) << " " << destTets(i, 1) << " " << destTets(i, 2)
           << " " << destTets(i, 3) << "\n";
    }

    std::cout << "Tetrahedralization sucess: " << destVertices.rows()
              << " nodes, " << destTets.rows() << " tetrahedra." << std::endl;
    std::cout << "Mesh saved: " << outFile << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error Usage: " << argv[0]
                  << " mesh[.off|.ply|.obj] [-out out_file]" << std::endl;
        return 0;
    }

    for (int i = 1; i < argc; ++i)
    {
        tetrahedralizeMesh(std::string(argv[i]));
    }

    return 0;

    // if (inFile.find(".off") != std::string::npos)
    // {
    //     igl::readOFF(inFile.c_str(), sVertices, sFacets);
    // }
    // else if (inFile.find(".ply") != std::string::npos)
    // {
    //     igl::readPLY(inFile.c_str(), sVertices, sFacets);
    // }
    // else if (inFile.find(".obj") != std::string::npos)
    // {
    //     igl::readOBJ(inFile.c_str(), sVertices, sFacets);
    // }
    // else
    // {
    //     return 0;
    // }

    // return 1;
}
