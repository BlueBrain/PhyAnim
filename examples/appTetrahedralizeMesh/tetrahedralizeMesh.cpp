/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <iostream>
// #include <cstdio>
// #include <igl/readOBJ.h>
// #include <igl/writeOFF.h>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/readOFF.h>
#include <igl/readPLY.h>
#include <phyanim/Phyanim.h>

#include <fstream>

using namespace phyanim;

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

        auto mesh = new geometry::Mesh();
        mesh->load(file);

        uint32_t size = mesh->nodes.size();
        sourceVertices = Eigen::MatrixXd(size, 3);
        for (uint32_t i = 0; i < size; ++i)
        {
            sourceVertices(i, 0) = mesh->nodes[i]->position.x;
            sourceVertices(i, 1) = mesh->nodes[i]->position.y;
            sourceVertices(i, 2) = mesh->nodes[i]->position.z;
        }

        size = mesh->triangles.size();
        sourceFacets = Eigen::MatrixXi(size, 3);
        for (uint32_t i = 0; i < size; ++i)
        {
            auto triangle =
                dynamic_cast<geometry::Triangle*>(mesh->triangles[i]);
            sourceFacets(i, 0) = triangle->node0->id;
            sourceFacets(i, 1) = triangle->node1->id;
            sourceFacets(i, 2) = triangle->node2->id;
        }
        delete mesh;
    }
    else
    {
        return;
    }

    outFile.append(".tet");
    igl::copyleft::tetgen::tetrahedralize(sourceVertices, sourceFacets,
                                          "Qa0.01", destVertices, destTets,
                                          destFacets);

    if (destVertices.rows() == 0 || destTets.rows() == 0)
    {
        std::cerr << "Tetrahedralization failed. Not file saved: " << file
                  << std::endl;
        return;
    }

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
    os.close();

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
