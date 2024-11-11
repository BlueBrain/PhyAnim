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

#include "ImplicitFEMSystem.h"

#include <Eigen/Dense>
#include <iostream>

#include "../geometry/Tetrahedron.h"
#include "../geometry/Triangle.h"

namespace phyanim
{
namespace anim
{
ImplicitFEMSystem::ImplicitFEMSystem(float dt,
                                     CollisionDetection* collDetector_)
    : AnimSystem(dt)
{
}

ImplicitFEMSystem::~ImplicitFEMSystem(void) {}

void ImplicitFEMSystem::preprocessMesh(geometry::MeshPtr mesh_)
{
    _conformKMatrix(mesh_);
}

void ImplicitFEMSystem::_step(geometry::MeshPtr mesh)
{
    geometry::Nodes& nodes = mesh->nodes;
    uint64_t size = nodes.size() * 3;
    Eigen::VectorXf u(size);
    Eigen::VectorXf mv(size);
    Eigen::VectorXf fext(size);
    Eigen::VectorXf v_1(size);
    Eigen::VectorXf b(size);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < size / 3; ++i)
    {
        geometry::Node* node = mesh->nodes[i];
        _addVec3ToVecX(i, node->position - node->initPosition, u);
        _addVec3ToVecX(i, node->velocity * node->mass, mv);
        _addVec3ToVecX(i, node->force, fext);
    }

    b = mv - _dt * (mesh->kMatrix * u - fext);
    v_1 = mesh->AMatrixSolver.solve(b);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < size / 3; ++i)
    {
        geometry::Node* node = mesh->nodes[i];
        if (!node->fix)
        {
            geometry::Vec3 v(v_1[i * 3], v_1[i * 3 + 1], v_1[i * 3 + 2]);
            geometry::Vec3 x = node->position + v * _dt;
            node->velocity = v;
            node->position = x;
        }
    }
}

void ImplicitFEMSystem::_conformKMatrix(geometry::MeshPtr mesh)
{
    float young = mesh->stiffness;
    float poisson = mesh->poissonRatio;
    float D = young / ((1 + poisson) * (1 - 2 * poisson));
    float D0 = D * (1 - poisson);
    float D1 = D * poisson;
    float D2 = D * (1 - 2 * poisson) * 0.5;
    float dt2 = _dt * _dt;
    TKs ks;
    _computeTetsK(mesh->tetrahedra, ks, D0, D1, D2);

    geometry::Nodes& nodes = mesh->nodes;
    uint64_t size = nodes.size() * 3;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < size / 3; ++i)
    {
        nodes[i]->id = i;
    }

    Triplets kTriplets(mesh->tetrahedra.size() * 16 * 9);
    Triplets aTriplets(kTriplets.size() + nodes.size() * 3);
    _buildKTriplets(mesh->tetrahedra, ks, dt2, kTriplets, aTriplets);

    uint64_t tripletId = kTriplets.size();
    for (uint64_t i = 0; i < size / 3; ++i)
    {
        _addIdentityValueToTriplets(i, nodes[i]->mass, aTriplets,
                                    tripletId + i * 3);
    }

    mesh->kMatrix.resize(size, size);
    mesh->AMatrix.resize(size, size);
    mesh->kMatrix.resizeNonZeros(kTriplets.size());
    mesh->AMatrix.resizeNonZeros(aTriplets.size());
    mesh->kMatrix.setFromTriplets(kTriplets.begin(), kTriplets.end());
    mesh->AMatrix.setFromTriplets(aTriplets.begin(), aTriplets.end());

    kTriplets.clear();
    aTriplets.clear();
    mesh->AMatrixSolver.compute(mesh->AMatrix);
}

void ImplicitFEMSystem::_buildKTriplets(const geometry::Primitives& tets,
                                        TKs& ks,
                                        float dt2,
                                        Triplets& kTriplets,
                                        Triplets& aTriplets)
{
    for (uint64_t i = 0; i < tets.size(); ++i)
    {
        auto tet = dynamic_cast<geometry::TetrahedronPtr>(tets[i]);
        uint64_t id0 = tet->node0->id;
        uint64_t id1 = tet->node1->id;
        uint64_t id2 = tet->node2->id;
        uint64_t id3 = tet->node3->id;

        uint64_t tripletId = i * 16 * 9;

        // row 0
        Eigen::Matrix3f k = ks[i].k00;
        _addMatrixToTriplets(id0, id0, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id0, id0, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k01;
        _addMatrixToTriplets(id0, id1, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id0, id1, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k02;
        _addMatrixToTriplets(id0, id2, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id0, id2, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k03;
        _addMatrixToTriplets(id0, id3, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id0, id3, k, aTriplets, tripletId);
        // row 1
        tripletId += 9;
        k = ks[i].k01.transpose();
        _addMatrixToTriplets(id1, id0, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id1, id0, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k11;
        _addMatrixToTriplets(id1, id1, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id1, id1, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k12;
        _addMatrixToTriplets(id1, id2, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id1, id2, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k13;
        _addMatrixToTriplets(id1, id3, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id1, id3, k, aTriplets, tripletId);
        // row 2
        tripletId += 9;
        k = ks[i].k02.transpose();
        _addMatrixToTriplets(id2, id0, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id2, id0, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k12.transpose();
        _addMatrixToTriplets(id2, id1, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id2, id1, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k22;
        _addMatrixToTriplets(id2, id2, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id2, id2, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k23;
        _addMatrixToTriplets(id2, id3, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id2, id3, k, aTriplets, tripletId);
        // row 3
        tripletId += 9;
        k = ks[i].k03.transpose();
        _addMatrixToTriplets(id3, id0, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id3, id0, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k13.transpose();
        _addMatrixToTriplets(id3, id1, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id3, id1, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k23.transpose();
        _addMatrixToTriplets(id3, id2, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id3, id2, k, aTriplets, tripletId);
        tripletId += 9;
        k = ks[i].k33;
        _addMatrixToTriplets(id3, id3, k, kTriplets, tripletId);
        k *= dt2;
        _addMatrixToTriplets(id3, id3, k, aTriplets, tripletId);
    }
}

void ImplicitFEMSystem::_computeTetsK(const geometry::Primitives& tets,
                                      TKs& ks,
                                      float D0,
                                      float D1,
                                      float D2)
{
    Eigen::MatrixXf D(6, 6);
    D << D0, D1, D1, 0.0, 0.0, 0.0, D1, D0, D1, 0.0, 0.0, 0.0, D1, D1, D0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, D2;

    ks.resize(tets.size());
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < tets.size(); ++i)
    {
        auto tet = dynamic_cast<geometry::TetrahedronPtr>(tets[i]);

        Eigen::Vector3f x0(glm::value_ptr(tet->node0->initPosition));
        Eigen::Vector3f x1(glm::value_ptr(tet->node1->initPosition));
        Eigen::Vector3f x2(glm::value_ptr(tet->node2->initPosition));
        Eigen::Vector3f x3(glm::value_ptr(tet->node3->initPosition));

        Eigen::Matrix3f basis;
        basis << x1 - x0, x2 - x0, x3 - x0;
        basis = basis.inverse().eval();

        Eigen::Vector3f b1 = basis.row(0);
        Eigen::Vector3f b2 = basis.row(1);
        Eigen::Vector3f b3 = basis.row(2);
        Eigen::Vector3f b0 = -b1 - b2 - b3;

        Eigen::MatrixXf B0(6, 3);
        B0 << b0[0], 0.0, 0.0, 0.0, b0[1], 0.0, 0.0, 0.0, b0[2], b0[1], b0[0],
            0.0, 0.0, b0[2], b0[1], b0[2], 0.0, b0[0];
        Eigen::MatrixXf B0T = B0.transpose();

        Eigen::MatrixXf B1(6, 3);
        B1 << b1[0], 0.0, 0.0, 0.0, b1[1], 0.0, 0.0, 0.0, b1[2], b1[1], b1[0],
            0.0, 0.0, b1[2], b1[1], b1[2], 0.0, b1[0];
        Eigen::MatrixXf B1T = B1.transpose();

        Eigen::MatrixXf B2(6, 3);
        B2 << b2[0], 0.0, 0.0, 0.0, b2[1], 0.0, 0.0, 0.0, b2[2], b2[1], b2[0],
            0.0, 0.0, b2[2], b2[1], b2[2], 0.0, b2[0];

        Eigen::MatrixXf B2T = B2.transpose();

        Eigen::MatrixXf B3(6, 3);
        B3 << b3[0], 0.0, 0.0, 0.0, b3[1], 0.0, 0.0, 0.0, b3[2], b3[1], b3[0],
            0.0, 0.0, b3[2], b3[1], b3[2], 0.0, b3[0];
        Eigen::MatrixXf B3T = B3.transpose();

        float volume = tet->initVolume();

        ks[i].k00 = B0T * D * B0 * volume;
        ks[i].k11 = B1T * D * B1 * volume;
        ks[i].k22 = B2T * D * B2 * volume;
        ks[i].k33 = B3T * D * B3 * volume;
        ks[i].k01 = B0T * D * B1 * volume;
        ks[i].k02 = B0T * D * B2 * volume;
        ks[i].k03 = B0T * D * B3 * volume;
        ks[i].k12 = B1T * D * B2 * volume;
        ks[i].k13 = B1T * D * B3 * volume;
        ks[i].k23 = B2T * D * B3 * volume;
    }
}

void ImplicitFEMSystem::_addMatrixToTriplets(uint64_t id0,
                                             uint64_t id1,
                                             const Eigen::Matrix3f& m,
                                             Triplets& triplets,
                                             uint64_t tripletId)
{
    id0 *= 3;
    id1 *= 3;

    for (uint64_t i = 0; i < 3; ++i)
    {
        for (uint64_t j = 0; j < 3; ++j)
        {
            triplets[tripletId + (i * 3 + j)] =
                Triplet(id0 + i, id1 + j, m(i, j));
        }
    }
}

void ImplicitFEMSystem::_addIdentityValueToTriplets(uint64_t id0,
                                                    float value,
                                                    Triplets& triplets,
                                                    uint64_t tripletId)
{
    id0 *= 3;

    for (uint64_t i = 0; i < 3; ++i)
    {
        triplets[tripletId + i] = Triplet(id0 + i, id0 + i, value);
    }
}
void ImplicitFEMSystem::_addVec3ToVecX(uint64_t id,
                                       const geometry::Vec3& value,
                                       Eigen::VectorXf& vecx)
{
    id *= 3;
    for (uint64_t i = 0; i < 3; ++i)
    {
        vecx[id + i] = value[i];
    }
}

}  // namespace anim
}  // namespace phyanim
