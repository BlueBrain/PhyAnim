#include <ImplicitMassSpringSystem.h>

#include <iostream>

namespace phyanim
{
ImplicitMassSpringSystem::ImplicitMassSpringSystem(double dt) : AnimSystem(dt)
{
}

ImplicitMassSpringSystem::~ImplicitMassSpringSystem() {}

void ImplicitMassSpringSystem::_step()
{
    double dt2 = _dt * _dt;
    for (unsigned int i = 0; i < _meshes.size(); ++i)
    {
        auto mesh = _meshes[i];
        auto ks = mesh->stiffness;
        auto kd = 1000.0 * mesh->damping;

        auto& nodes = mesh->nodes;
        unsigned int n = nodes.size();
        Eigen::MatrixXd A(n * 3, n * 3);
        for (unsigned int mI = 0; mI < n * 3; ++mI)
        {
            for (unsigned int mJ = 0; mJ < n * 3; ++mJ)
            {
                A(mI, mJ) = 0.0;
            }
        }
        Eigen::VectorXd b(n * 3);
        Eigen::VectorXd vIncVec(n * 3);

        for (unsigned int nodeId = 0; nodeId < n; ++nodeId)
        {
            float mass = nodes[nodeId]->mass;
            Vec3 fext = nodes[nodeId]->force;
            b(nodeId * 3) = fext.x() * _dt;
            b(nodeId * 3 + 1) = fext.y() * _dt;
            b(nodeId * 3 + 2) = fext.z() * _dt;

            A(nodeId * 3, nodeId * 3) = mass;
            A(nodeId * 3 + 1, nodeId * 3 + 1) = mass;
            A(nodeId * 3 + 2, nodeId * 3 + 2) = mass;
            // coeffs.push_back(Td(mass, nodeId*3));
            // coeffs.push_back(Td(mass, nodeId*3+1));
            // coeffs.push_back(Td(mass, nodeId*3+2));
        }

        for (auto edge : mesh->edges)
        {
            unsigned int idI = edge->node0->id;
            unsigned int idJ = edge->node1->id;
            auto nodeI = edge->node0;
            auto nodeJ = edge->node1;
            Vec3 dij = nodeJ->position - nodeI->position;
            Vec3 dvij = nodeJ->velocity - nodeI->velocity;

            double r = edge->resLength;
            double rI = r;
            if (rI != 0)
            {
                rI = 1.0 / rI;
            }
            double l = dij.norm();
            double lI = l;
            double lI2 = l;
            if (lI != 0)
            {
                lI = 1.0 / lI;
                lI2 = lI * lI;
            }

            // Vec3 f0 = (ks*(l*dr - 1.0) + kd*(v.dot(dx)*dl*dr)) * dx*dl;
            Vec3 fij = ks * (1.0 - r * lI) * dij + dvij * kd;

            b[idI * 3] += fij.x() * _dt;
            b[idI * 3 + 1] += fij.y() * _dt;
            b[idI * 3 + 2] += fij.z() * _dt;

            b[idJ * 3] -= fij.x() * _dt;
            b[idJ * 3 + 1] -= fij.y() * _dt;
            b[idJ * 3 + 2] -= fij.z() * _dt;

            Mat3 outter = dij * dij.transpose() * lI2;
            Mat3 Jij = ks * (outter + (Eigen::Matrix3d::Identity() - outter) *
                                          (1 - r * lI));
            Mat3 Jii = (-1.0) * Jij;

            Vec3 fJij = Jij * dvij * dt2;
            b[idI * 3] += fJij.x();
            b[idI * 3 + 1] += fJij.y();
            b[idI * 3 + 2] += fJij.z();
            b[idI * 3] -= fJij.x();
            b[idI * 3 + 1] -= fJij.y();
            b[idI * 3 + 2] -= fJij.z();

            Jij = -dt2 * Jij;
            A.block<3, 3>(idI * 3, idJ * 3) = Jij;
            A.block<3, 3>(idJ * 3, idI * 3) = Jij;
            Jii = -dt2 * Jii;
            _addBlockToMatrix(Jii, A, idI * 3, idI * 3);
            _addBlockToMatrix(Jii, A, idJ * 3, idJ * 3);

            Mat3 Jvij = Mat3::Identity() * (-kd) * _dt;
            _addBlockToMatrix(Jvij, A, idI * 3, idJ * 3);
            _addBlockToMatrix(Jvij, A, idJ * 3, idI * 3);
            Jvij = (-1.0) * Jvij;
            _addBlockToMatrix(Jvij, A, idI * 3, idI * 3);
            _addBlockToMatrix(Jvij, A, idJ * 3, idJ * 3);
        }

        // auto massCoeffs = _massCoeffs[i];

        // coeffs.insert(coeffs.end(), massCoeffs.begin( ), massCoeffs.end());

        // A.setFromTriplets(coeffs.begin(), coeffs.end());

        // Eigen::ConjugateGradient<Eigen::MatrixXd> cg;
        // cg.compute(A);

        vIncVec = A.colPivHouseholderQr().solve(b);

        for (unsigned int nodeId = 0; nodeId < n; ++nodeId)
        {
            auto node = nodes[nodeId];
            Vec3 vInc(vIncVec[nodeId * 3], vIncVec[nodeId * 3 + 1],
                      vIncVec[nodeId * 3 + 2]);
            // Vec3 vInc(b[nodeId*3],b[nodeId*3+1],b[nodeId*3+2]);
            // vInc = vInc * (1.0/node->mass());
            Vec3 v = node->velocity + vInc;
            Vec3 x = node->position + v * _dt;
            node->velocity = v;
            node->position = x;
        }
    }
}

Tds ImplicitMassSpringSystem::_mat3ToTd(Mat3& mat_,
                                        unsigned int i_,
                                        unsigned int j_)
{
    Tds triplets(9);
    for (unsigned int i = 0; i < 3; ++i)
    {
        for (unsigned int j = 0; j < 3; ++j)
        {
            triplets[i + j * 3] = Td(i_ * 3 + i, j_ * 3 + j, mat_(i, j));
        }
    }
    return triplets;
}

void ImplicitMassSpringSystem::_addBlockToMatrix(Mat3& block_,
                                                 Eigen::MatrixXd& mat_,
                                                 unsigned int i_,
                                                 unsigned int j_)
{
    for (unsigned int i = 0; i < 3; ++i)
    {
        for (unsigned int j = 0; j < 3; ++j)
        {
            mat_(i_ + i, j_ + j) += block_(i, j);
        }
    }
}

Tds ImplicitMassSpringSystem::_diagVec3ToTd(Vec3& vec_,
                                            unsigned int i_,
                                            unsigned int j_)
{
    Tds triplets(3);
    triplets[0] = Td(i_ * 3, j_ * 3, vec_.x());
    triplets[1] = Td(i_ * 3 + 1, j_ * 3 + 1, vec_.x());
    triplets[2] = Td(i_ * 3 + 2, j_ * 3 + 2, vec_.x());
    return triplets;
}

}  // namespace phyanim
