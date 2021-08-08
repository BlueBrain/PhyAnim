#include "ExplicitFEMSystem.h"

#include <Eigen/Dense>
#include <iostream>

#include "Tetrahedron.h"
#include "Triangle.h"

namespace phyanim
{
ExplicitFEMSystem::ExplicitFEMSystem(double dt) : AnimSystem(dt) {}

ExplicitFEMSystem::~ExplicitFEMSystem(void) {}

void ExplicitFEMSystem::_step(Mesh* mesh)
{
    double kYoung = mesh->stiffness;
    double kPoisson = mesh->poissonRatio;
    double lambda = (kPoisson * kYoung) / ((1 + kPoisson) * (1 - 2 * kPoisson));
    double mu = kYoung / (2 * (1 + kPoisson));
    double damp = mesh->damping;

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < mesh->tetrahedra.size(); i++)
    {
        auto tet = dynamic_cast<TetrahedronPtr>(mesh->tetrahedra[i]);
        Mat3 x, f, q, fTilde, strain, stress;

        Mat3 xdot, fdot, fdotTilde, strainrate, stressrate;

        Vec3 x0 = tet->node0->position;
        Vec3 x1 = tet->node1->position;
        Vec3 x2 = tet->node2->position;
        Vec3 x3 = tet->node3->position;

        Vec3 v0 = tet->node0->velocity;
        Vec3 v1 = tet->node1->velocity;
        Vec3 v2 = tet->node2->velocity;
        Vec3 v3 = tet->node3->velocity;

        x << x1 - x0, x2 - x0, x3 - x0;
        f = x * tet->invBasis;
        _polar(f, q);
        fTilde = q.transpose() * f;
        strain = 0.5 * (fTilde + fTilde.transpose()) - Mat3::Identity();
        stress = lambda * strain.trace() * Mat3::Identity() + 2.0 * mu * strain;

        xdot << v1 - v0, v2 - v0, v3 - v0;
        fdot = xdot * tet->invBasis;
        fdotTilde = q.transpose() * fdot;
        strainrate =
            0.5 * (fdotTilde + fdotTilde.transpose()) - Mat3::Identity();
        stressrate = damp * (lambda * strainrate.trace() * Mat3::Identity() +
                             2.0 * mu * strainrate);

        tet->node0->force += q * (stress + stressrate) * -tet->normal0 / 0.6;
        tet->node1->force += q * (stress + stressrate) * -tet->normal1 / 0.6;
        tet->node2->force += q * (stress + stressrate) * -tet->normal2 / 0.6;
        tet->node3->force += q * (stress + stressrate) * -tet->normal3 / 0.6;
    }

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < mesh->nodes.size(); i++)
    {
        auto node = mesh->nodes[i];
        if (!node->fix)
        {
            Vec3 a = node->force / node->mass;
            Vec3 v = node->velocity + a * _dt;
            Vec3 x = node->position + v * _dt;
            node->velocity = v;
            node->position = x;
        }
    }
}

void ExplicitFEMSystem::_polar(const Mat3& f_, Mat3& q_) const
{
    Eigen::JacobiSVD<Mat3> svd(f_, Eigen::ComputeFullU | Eigen::ComputeFullV);
    auto u = svd.matrixU();
    auto v = svd.matrixV();
    q_ = u * v.transpose();
}

}  // namespace phyanim
