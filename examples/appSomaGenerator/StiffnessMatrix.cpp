#include "StiffnessMatrix.h"

namespace examples
{
StiffnessMatrix::StiffnessMatrix(){};

void StiffnessMatrix::computeMatrices(Nodes nodes,
                                      Tets tets,
                                      double stiffness,
                                      double poissonRatio,
                                      double dt)
{
    double D = stiffness / ((1 + poissonRatio) * (1 - 2 * poissonRatio));
    double D0 = D * (1 - poissonRatio);
    double D1 = D * poissonRatio;
    double D2 = D * (1 - 2 * poissonRatio) * 0.5;
    Eigen::MatrixXd mD(6, 6);
    mD << D0, D1, D1, 0.0, 0.0, 0.0, D1, D0, D1, 0.0, 0.0, 0.0, D1, D1, D0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, D2;
    double dt2 = dt * dt;

    KMats kMats(tets.size());
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < nodes.size(); ++i) nodes[i]->id = i;

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < tets.size(); ++i)
        _computeTet(tets[i], kMats[i], mD);

    Triplets kTriplets;
    Triplets aTriplets;
    _buildKTriplets(tets, kMats, dt2, kTriplets, aTriplets);

    for (uint64_t i = 0; i < nodes.size(); ++i)
    {
        _addIdentityValueToTriplets(i, nodes[i]->mass, aTriplets);
    }

    kMatrix.resize(nodes.size() * 3, nodes.size() * 3);
    aMatrix.resize(nodes.size() * 3, nodes.size() * 3);
    kMatrix.resizeNonZeros(kTriplets.size());
    aMatrix.resizeNonZeros(aTriplets.size());
    kMatrix.setFromTriplets(kTriplets.begin(), kTriplets.end());
    aMatrix.setFromTriplets(aTriplets.begin(), aTriplets.end());

    kTriplets.clear();
    aTriplets.clear();
    aMatrixSolver.compute(aMatrix);
}

void StiffnessMatrix::_computeTet(TetPtr tet, KMat& kMat, Eigen::MatrixXd D)
{
    phyanim::Vec3 x0 = tet->node0->initPosition;
    phyanim::Vec3 x1 = tet->node1->initPosition;
    phyanim::Vec3 x2 = tet->node2->initPosition;
    phyanim::Vec3 x3 = tet->node3->initPosition;

    phyanim::Mat3 basis;
    basis << x1 - x0, x2 - x0, x3 - x0;
    basis = basis.inverse().eval();

    phyanim::Vec3 b1 = basis.row(0);
    phyanim::Vec3 b2 = basis.row(1);
    phyanim::Vec3 b3 = basis.row(2);
    phyanim::Vec3 b0 = -b1 - b2 - b3;

    Eigen::MatrixXd B0(6, 3);
    B0 << b0[0], 0.0, 0.0, 0.0, b0[1], 0.0, 0.0, 0.0, b0[2], b0[1], b0[0], 0.0,
        0.0, b0[2], b0[1], b0[2], 0.0, b0[0];
    Eigen::MatrixXd B0T = B0.transpose();

    Eigen::MatrixXd B1(6, 3);
    B1 << b1[0], 0.0, 0.0, 0.0, b1[1], 0.0, 0.0, 0.0, b1[2], b1[1], b1[0], 0.0,
        0.0, b1[2], b1[1], b1[2], 0.0, b1[0];
    Eigen::MatrixXd B1T = B1.transpose();

    Eigen::MatrixXd B2(6, 3);
    B2 << b2[0], 0.0, 0.0, 0.0, b2[1], 0.0, 0.0, 0.0, b2[2], b2[1], b2[0], 0.0,
        0.0, b2[2], b2[1], b2[2], 0.0, b2[0];

    Eigen::MatrixXd B2T = B2.transpose();

    Eigen::MatrixXd B3(6, 3);
    B3 << b3[0], 0.0, 0.0, 0.0, b3[1], 0.0, 0.0, 0.0, b3[2], b3[1], b3[0], 0.0,
        0.0, b3[2], b3[1], b3[2], 0.0, b3[0];
    Eigen::MatrixXd B3T = B3.transpose();

    double volume = tet->initVolume();

    kMat.k00 = B0T * D * B0 * volume;
    kMat.k11 = B1T * D * B1 * volume;
    kMat.k22 = B2T * D * B2 * volume;
    kMat.k33 = B3T * D * B3 * volume;
    kMat.k01 = B0T * D * B1 * volume;
    kMat.k02 = B0T * D * B2 * volume;
    kMat.k03 = B0T * D * B3 * volume;
    kMat.k12 = B1T * D * B2 * volume;
    kMat.k13 = B1T * D * B3 * volume;
    kMat.k23 = B2T * D * B3 * volume;
}

void StiffnessMatrix::_buildKTriplets(Tets tets,
                                      KMats& kMats,
                                      double dt2,
                                      Triplets& kTriplets,
                                      Triplets& aTriplets)
{
    for (uint64_t i = 0; i < tets.size(); ++i)
    {
        auto tet = tets[i];
        uint64_t id0 = tet->node0->id;
        uint64_t id1 = tet->node1->id;
        uint64_t id2 = tet->node2->id;
        uint64_t id3 = tet->node3->id;

        // row 0
        phyanim::Mat3 k = kMats[i].k00;
        _addMatrixToTriplets(id0, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id0, k, aTriplets);
        k = kMats[i].k01;
        _addMatrixToTriplets(id0, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id1, k, aTriplets);
        k = kMats[i].k02;
        _addMatrixToTriplets(id0, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id2, k, aTriplets);
        k = kMats[i].k03;
        _addMatrixToTriplets(id0, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id3, k, aTriplets);
        // row 1
        k = kMats[i].k01.transpose();
        _addMatrixToTriplets(id1, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id0, k, aTriplets);
        k = kMats[i].k11;
        _addMatrixToTriplets(id1, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id1, k, aTriplets);
        k = kMats[i].k12;
        _addMatrixToTriplets(id1, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id2, k, aTriplets);
        k = kMats[i].k13;
        _addMatrixToTriplets(id1, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id3, k, aTriplets);
        // row 2
        k = kMats[i].k02.transpose();
        _addMatrixToTriplets(id2, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id0, k, aTriplets);
        k = kMats[i].k12.transpose();
        _addMatrixToTriplets(id2, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id1, k, aTriplets);
        k = kMats[i].k22;
        _addMatrixToTriplets(id2, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id2, k, aTriplets);
        k = kMats[i].k23;
        _addMatrixToTriplets(id2, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id3, k, aTriplets);
        // row 3
        k = kMats[i].k03.transpose();
        _addMatrixToTriplets(id3, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id0, k, aTriplets);
        k = kMats[i].k13.transpose();
        _addMatrixToTriplets(id3, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id1, k, aTriplets);
        k = kMats[i].k23.transpose();
        _addMatrixToTriplets(id3, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id2, k, aTriplets);
        k = kMats[i].k33;
        _addMatrixToTriplets(id3, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id3, k, aTriplets);
    }
}

void StiffnessMatrix::_addMatrixToTriplets(uint64_t id0,
                                           uint64_t id1,
                                           const phyanim::Mat3& m,
                                           Triplets& triplets)
{
    id0 *= 3;
    id1 *= 3;
    for (uint64_t i = 0; i < 3; ++i)
    {
        for (uint64_t j = 0; j < 3; ++j)
        {
            triplets.push_back(Triplet(id0 + i, id1 + j, m(i, j)));
        }
    }
}

void StiffnessMatrix::_addIdentityValueToTriplets(uint64_t id0,
                                                  double value,
                                                  Triplets& triplets)
{
    id0 *= 3;
    for (uint64_t i = 0; i < 3; ++i)
    {
        triplets.push_back(Triplet(id0 + i, id0 + i, value));
    }
}

}  // namespace examples
