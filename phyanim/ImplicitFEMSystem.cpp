#include "ImplicitFEMSystem.h"

#include <Eigen/Dense>
#include <iostream>

#include "Tetrahedron.h"
#include "Triangle.h"

namespace phyanim
{
ImplicitFEMSystem::ImplicitFEMSystem(double dt,
                                     CollisionDetection* collDetector_)
    : AnimSystem(dt)
{
}

ImplicitFEMSystem::~ImplicitFEMSystem(void) {}

void ImplicitFEMSystem::preprocessMesh(Mesh* mesh_) { _conformKMatrix(mesh_); }

void ImplicitFEMSystem::_step(Mesh* mesh)
{
    Nodes& nodes = mesh->nodes;
    uint64_t size = nodes.size() * 3;
    Eigen::VectorXd u(size);
    Eigen::VectorXd mv(size);
    Eigen::VectorXd fext(size);
    Eigen::VectorXd v_1(size);
    Eigen::VectorXd b(size);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < size / 3; ++i)
    {
        Node* node = mesh->nodes[i];
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
        Node* node = mesh->nodes[i];
        if (!node->fix)
        {
            Vec3 v(v_1[i * 3], v_1[i * 3 + 1], v_1[i * 3 + 2]);
            Vec3 x = node->position + v * _dt;
            node->velocity = v;
            node->position = x;
        }
    }
}

void ImplicitFEMSystem::_conformKMatrix(Mesh* mesh)
{
    double young = mesh->stiffness;
    double poisson = mesh->poissonRatio;
    double D = young / ((1 + poisson) * (1 - 2 * poisson));
    double D0 = D * (1 - poisson);
    double D1 = D * poisson;
    double D2 = D * (1 - 2 * poisson) * 0.5;
    double dt2 = _dt * _dt;
    TKs ks;
    _computeTetsK(mesh->tetrahedra, ks, D0, D1, D2);

    Nodes& nodes = mesh->nodes;
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

void ImplicitFEMSystem::_buildKTriplets(const Primitives& tets,
                                        TKs& ks,
                                        double dt2,
                                        Triplets& kTriplets,
                                        Triplets& aTriplets)
{
    for (uint64_t i = 0; i < tets.size(); ++i)
    {
        auto tet = dynamic_cast<TetrahedronPtr>(tets[i]);
        uint64_t id0 = tet->node0->id;
        uint64_t id1 = tet->node1->id;
        uint64_t id2 = tet->node2->id;
        uint64_t id3 = tet->node3->id;

        uint64_t tripletId = i * 16 * 9;

        // row 0
        Mat3 k = ks[i].k00;
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

void ImplicitFEMSystem::_computeTetsK(const Primitives& tets,
                                      TKs& ks,
                                      double D0,
                                      double D1,
                                      double D2)
{
    Eigen::MatrixXd D(6, 6);
    D << D0, D1, D1, 0.0, 0.0, 0.0, D1, D0, D1, 0.0, 0.0, 0.0, D1, D1, D0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, D2;

    ks.resize(tets.size());
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < tets.size(); ++i)
    {
        auto tet = dynamic_cast<TetrahedronPtr>(tets[i]);

        Vec3 x0 = tet->node0->initPosition;
        Vec3 x1 = tet->node1->initPosition;
        Vec3 x2 = tet->node2->initPosition;
        Vec3 x3 = tet->node3->initPosition;

        Mat3 basis;
        basis << x1 - x0, x2 - x0, x3 - x0;
        basis = basis.inverse().eval();

        Vec3 b1 = basis.row(0);
        Vec3 b2 = basis.row(1);
        Vec3 b3 = basis.row(2);
        Vec3 b0 = -b1 - b2 - b3;

        Eigen::MatrixXd B0(6, 3);
        B0 << b0[0], 0.0, 0.0, 0.0, b0[1], 0.0, 0.0, 0.0, b0[2], b0[1], b0[0],
            0.0, 0.0, b0[2], b0[1], b0[2], 0.0, b0[0];
        Eigen::MatrixXd B0T = B0.transpose();

        Eigen::MatrixXd B1(6, 3);
        B1 << b1[0], 0.0, 0.0, 0.0, b1[1], 0.0, 0.0, 0.0, b1[2], b1[1], b1[0],
            0.0, 0.0, b1[2], b1[1], b1[2], 0.0, b1[0];
        Eigen::MatrixXd B1T = B1.transpose();

        Eigen::MatrixXd B2(6, 3);
        B2 << b2[0], 0.0, 0.0, 0.0, b2[1], 0.0, 0.0, 0.0, b2[2], b2[1], b2[0],
            0.0, 0.0, b2[2], b2[1], b2[2], 0.0, b2[0];

        Eigen::MatrixXd B2T = B2.transpose();

        Eigen::MatrixXd B3(6, 3);
        B3 << b3[0], 0.0, 0.0, 0.0, b3[1], 0.0, 0.0, 0.0, b3[2], b3[1], b3[0],
            0.0, 0.0, b3[2], b3[1], b3[2], 0.0, b3[0];
        Eigen::MatrixXd B3T = B3.transpose();

        double volume = tet->initVolume();

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
                                             const Mat3& m,
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
                                                    double value,
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
                                       const Vec3& value,
                                       Eigen::VectorXd& vecx)
{
    id *= 3;
    for (uint64_t i = 0; i < 3; ++i)
    {
        vecx[id + i] = value[i];
    }
}

}  // namespace phyanim
