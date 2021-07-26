#include <ImplicitFEMSystem.h>

#include <Eigen/Dense>
#include <iostream>

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
        if (!node->fixed)
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
    _computeTetsK(mesh->tetrahedra, D0, D1, D2);

    Nodes& nodes = mesh->nodes;
    uint64_t size = nodes.size() * 3;
    for (uint64_t i = 0; i < size / 3; ++i)
    {
        nodes[i]->id = i;
    }

    Triplets kTriplets;
    Triplets ATriplets;
    _buildKTriplets(mesh->tetrahedra, dt2, kTriplets, ATriplets);
    for (uint64_t i = 0; i < size / 3; ++i)
    {
        _addIdentityValueToTriplets(i, nodes[i]->mass, ATriplets);
    }

    mesh->kMatrix.resize(size, size);
    mesh->AMatrix.resize(size, size);
    mesh->kMatrix.resizeNonZeros(kTriplets.size());
    mesh->AMatrix.resizeNonZeros(ATriplets.size());
    mesh->kMatrix.setFromTriplets(kTriplets.begin(), kTriplets.end());
    mesh->AMatrix.setFromTriplets(ATriplets.begin(), ATriplets.end());

    kTriplets.clear();
    ATriplets.clear();
    mesh->AMatrixSolver.compute(mesh->AMatrix);
}

void ImplicitFEMSystem::_buildKTriplets(const Tetrahedra& tets,
                                        double dt2,
                                        Triplets& kTriplets,
                                        Triplets& ATriplets)
{
    for (auto tet : tets)
    {
        uint64_t id0 = tet->node0->id;
        uint64_t id1 = tet->node1->id;
        uint64_t id2 = tet->node2->id;
        uint64_t id3 = tet->node3->id;

        // row 0
        Mat3 k = tet->k00;
        _addMatrixToTriplets(id0, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id0, k, ATriplets);
        k = tet->k01;
        _addMatrixToTriplets(id0, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id1, k, ATriplets);
        k = tet->k02;
        _addMatrixToTriplets(id0, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id2, k, ATriplets);
        k = tet->k03;
        _addMatrixToTriplets(id0, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id0, id3, k, ATriplets);
        // row 1
        k = tet->k01.transpose();
        _addMatrixToTriplets(id1, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id0, k, ATriplets);
        k = tet->k11;
        _addMatrixToTriplets(id1, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id1, k, ATriplets);
        k = tet->k12;
        _addMatrixToTriplets(id1, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id2, k, ATriplets);
        k = tet->k13;
        _addMatrixToTriplets(id1, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id1, id3, k, ATriplets);
        // row 2
        k = tet->k02.transpose();
        _addMatrixToTriplets(id2, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id0, k, ATriplets);
        k = tet->k12.transpose();
        _addMatrixToTriplets(id2, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id1, k, ATriplets);
        k = tet->k22;
        _addMatrixToTriplets(id2, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id2, k, ATriplets);
        k = tet->k23;
        _addMatrixToTriplets(id2, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id2, id3, k, ATriplets);
        // row 3
        k = tet->k03.transpose();
        _addMatrixToTriplets(id3, id0, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id0, k, ATriplets);
        k = tet->k13.transpose();
        _addMatrixToTriplets(id3, id1, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id1, k, ATriplets);
        k = tet->k23.transpose();
        _addMatrixToTriplets(id3, id2, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id2, k, ATriplets);
        k = tet->k33;
        _addMatrixToTriplets(id3, id3, k, kTriplets);
        k *= dt2;
        _addMatrixToTriplets(id3, id3, k, ATriplets);
    }
}

void ImplicitFEMSystem::_computeTetsK(const Tetrahedra& tets,
                                      double D0,
                                      double D1,
                                      double D2)
{
    Eigen::MatrixXd D(6, 6);
    D << D0, D1, D1, 0.0, 0.0, 0.0, D1, D0, D1, 0.0, 0.0, 0.0, D1, D1, D0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, D2, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, D2;
    for (auto tet : tets)
    {
        Vec3 b1 = tet->invBasis.row(0);
        Vec3 b2 = tet->invBasis.row(1);
        Vec3 b3 = tet->invBasis.row(2);
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

        double volume = tet->initVolume;

        tet->k00 = B0T * D * B0 * volume;
        tet->k11 = B1T * D * B1 * volume;
        tet->k22 = B2T * D * B2 * volume;
        tet->k33 = B3T * D * B3 * volume;
        tet->k01 = B0T * D * B1 * volume;
        tet->k02 = B0T * D * B2 * volume;
        tet->k03 = B0T * D * B3 * volume;
        tet->k12 = B1T * D * B2 * volume;
        tet->k13 = B1T * D * B3 * volume;
        tet->k23 = B2T * D * B3 * volume;

        // tet->k00 =  _buildTetK(b0, b0, D0, D1, D2, volume);
        // tet->k11 =  _buildTetK(b1, b1, D0, D1, D2, volume);
        // tet->k22 =  _buildTetK(b2, b2, D0, D1, D2, volume);
        // tet->k33 =  _buildTetK(b3, b3, D0, D1, D2, volume);
        // tet->k01 =  _buildTetK(b0, b1, D0, D1, D2, volume);
        // tet->k02 =  _buildTetK(b0, b2, D0, D1, D2, volume);
        // tet->k03 =  _buildTetK(b0, b3, D0, D1, D2, volume);
        // tet->k12 =  _buildTetK(b1, b2, D0, D1, D2, volume);
        // tet->k13 =  _buildTetK(b1, b3, D0, D1, D2, volume);
        // tet->k23 =  _buildTetK(b2, b3, D0, D1, D2, volume);
    }
}

Mat3 ImplicitFEMSystem::_buildTetK(Vec3 bn_,
                                   Vec3 bm_,
                                   double D0,
                                   double D1,
                                   double D2,
                                   double volume)
{
    double bn = bn_[0];
    double cn = bn_[1];
    double dn = bn_[2];
    double bm = bm_[0];
    double cm = bm_[1];
    double dm = bm_[2];
    Mat3 knm;
    knm << D0 * (bn * bm) + D2 * (cn * cm + dn * dm),
        D1 * (bn * cm) + D2 * (cn * bm), D1 * (bn * dm) + D2 * (dn * bm),
        D1 * (cn * bm) + D2 * (bn * cm),
        D0 * (cn * cm) + D2 * (bn * bm + dn * dm),
        D1 * (cn * dm) + D2 * (dn * cm), D1 * (dn * bm) + D2 * (bn * dm),
        D1 * (dn * cm) + D2 * (cn * dm),
        D0 * (dn * dm) + D2 * (bn * bm + cn * cm);
    knm *= volume;
    return knm;
}

void ImplicitFEMSystem::_addMatrixToTriplets(uint64_t id0,
                                             uint64_t id1,
                                             const Mat3& m,
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

void ImplicitFEMSystem::_addIdentityValueToTriplets(uint64_t id0,
                                                    double value,
                                                    Triplets& triplets)
{
    id0 *= 3;

    for (uint64_t i = 0; i < 3; ++i)
    {
        triplets.push_back(Triplet(id0 + i, id0 + i, value));
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
