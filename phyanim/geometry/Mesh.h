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

#ifndef __PHYANIM_MESH__
#define __PHYANIM_MESH__

#include <Eigen/Sparse>

#include "Edge.h"
#include "HierarchicalAABB.h"

namespace phyanim
{
namespace geometry
{
class Mesh;

typedef Mesh* MeshPtr;

typedef std::vector<MeshPtr> Meshes;

class Mesh
{
public:
    Mesh(float stiffness_ = 1000.0,
         float density_ = 1.0,
         float damping_ = 1.0,
         float poissonRatio_ = 0.49);

    virtual ~Mesh(void);

    virtual void load(const std::string& file_);

    virtual void load(const std::string& nodeFile_,
                      const std::string& eleFile_);

    void clearData();

    void compute(bool createEdges = false);

    void write(const std::string& file_);

    float volume(void);

    float area(void);

    void trianglesToNodes(void);

    void trianglesToEdges(void);

    void tetsToNodes(void);

    void tetsToEdges(void);

    void tetsToTriangles(void);

    void computePerNodeMass(void);

    void nodesToInitPos(void);

    void nodesForceZero(void);

    virtual Mesh* copy(bool surfaceTriangles_ = true,
                       bool triangles_ = true,
                       bool tetrahedra_ = true,
                       bool edges_ = true);

    void positionDifference(float& mean_,
                            float& max_,
                            float& min_,
                            float& rms_);

    void computeNormals();

    Nodes nodes;

    Primitives surfaceTriangles;

    Primitives triangles;

    Primitives tetrahedra;

    Edges edges;

    HierarchicalAABBPtr boundingBox;

    float initArea;

    float initVolume;

    float stiffness;

    float density;

    float damping;

    float poissonRatio;

    Eigen::SparseMatrix<float> kMatrix;
    Eigen::SparseMatrix<float> AMatrix;
    Eigen::ConjugateGradient<Eigen::SparseMatrix<float>> AMatrixSolver;

private:
    void _split(const std::string& string_,
                std::vector<std::string>& strings_,
                char delim_ = ' ');

    void _loadOBJ(const std::string& file_);

    void _loadOFF(const std::string& file_);

    void _loadPLY(const std::string& file_);

    void _loadTET(const std::string& file_);

    void _loadTETGEN(const std::string& nodeFile_, const std::string& eleFile_);

    void _writeOFF(const std::string& file_);
    void _writeOBJ(const std::string& file_);

    void _writeTET(const std::string& file_);

    bool _normalsLoaded;
};

}  // namespace geometry
}  // namespace phyanim

#endif
