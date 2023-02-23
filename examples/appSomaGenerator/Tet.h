#ifndef __EXAMPLES_TET__
#define __EXAMPLES_TET__

#include "Spring.h"

namespace examples
{
class Tet;

typedef Tet* TetPtr;

typedef std::vector<TetPtr> Tets;

class Tet
{
public:
    Tet(Node* node0, Node* node1, Node* node2, Node* node3)
        : node0(node0)
        , node1(node1)
        , node2(node2)
        , node3(node3)
    {
        _volume = _computeVolume();
    };

    virtual ~Tet(){};

    double initVolume() { return _volume; }

    double volume() { return _computeVolume(); };

    void computeForces(double stiffness, double poissonRatio = 0.3)
    {
        double lambda = (poissonRatio * stiffness) /
                        ((1 + poissonRatio) * (1 - 2 * poissonRatio));
        double mu = stiffness / (2 * (1 + poissonRatio));

        phyanim::Mat3 x, f, q, fTilde, strain, stress;
        phyanim::Vec3 x0, x1, x2, x3;

        x0 = node0->initPosition;
        x1 = node1->initPosition;
        x2 = node2->initPosition;
        x3 = node3->initPosition;

        phyanim::Mat3 basis;
        basis << x1 - x0, x2 - x0, x3 - x0;
        basis = basis.inverse().eval();

        x0 = node0->position;
        x1 = node1->position;
        x2 = node2->position;
        x3 = node3->position;

        x << x1 - x0, x2 - x0, x3 - x0;

        f = x * basis;
        _polar(f, q);
        fTilde = q.transpose() * f;
        strain =
            0.5 * (fTilde + fTilde.transpose()) - phyanim::Mat3::Identity();
        stress = lambda * strain.trace() * phyanim::Mat3::Identity() +
                 2.0 * mu * strain;

        phyanim::Vec3 normal0 = (x3 - x1).cross(x2 - x1).normalized();
        phyanim::Vec3 normal1 = (x2 - x0).cross(x3 - x0).normalized();
        phyanim::Vec3 normal2 = (x3 - x0).cross(x1 - x0).normalized();
        phyanim::Vec3 normal3 = (x1 - x0).cross(x2 - x0).normalized();
        force0 += q * stress * -normal0 / 0.6;
        force1 += q * stress * -normal1 / 0.6;
        force2 += q * stress * -normal2 / 0.6;
        force3 += q * stress * -normal3 / 0.6;
    };

    Node* node0;
    Node* node1;
    Node* node2;
    Node* node3;
    phyanim::Vec3 force0;
    phyanim::Vec3 force1;
    phyanim::Vec3 force2;
    phyanim::Vec3 force3;

protected:
    double _volume;

    double _computeVolume()
    {
        phyanim::Vec3 x0 = node0->position;
        phyanim::Vec3 x1 = node1->position;
        phyanim::Vec3 x2 = node2->position;
        phyanim::Vec3 x3 = node3->position;
        phyanim::Mat3 basis;
        basis << x1 - x0, x2 - x0, x3 - x0;

        return std::abs(basis.determinant() / 6.0);
    };

    void _polar(const phyanim::Mat3& f, phyanim::Mat3& q) const
    {
        Eigen::JacobiSVD<phyanim::Mat3> svd(
            f, Eigen::ComputeFullU | Eigen::ComputeFullV);
        auto u = svd.matrixU();
        auto v = svd.matrixV();
        q = u * v.transpose();
    }
};

}  // namespace examples

#endif
