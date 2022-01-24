#ifndef __EXAMPLES_STIFFNESS_MATRIX__
#define __EXAMPLES_STIFFNESS_MATRIX__

#include <Eigen/Sparse>

#include "Tet.h"

namespace examples
{
class StiffnessMatrix
{
public:
    StiffnessMatrix();

    void computeMatrices(Nodes nodes,
                         Tets tets,
                         double stiffness = 1000.0,
                         double poissonRatio = 0.3,
                         double dt = 0.01);

    Eigen::SparseMatrix<double> kMatrix;
    Eigen::SparseMatrix<double> aMatrix;
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> aMatrixSolver;

protected:
    typedef Eigen::Triplet<double> Triplet;
    typedef std::vector<Triplet> Triplets;
    typedef struct KMat
    {
        phyanim::Mat3 k00;
        phyanim::Mat3 k11;
        phyanim::Mat3 k22;
        phyanim::Mat3 k33;
        phyanim::Mat3 k01;
        phyanim::Mat3 k02;
        phyanim::Mat3 k03;
        phyanim::Mat3 k12;
        phyanim::Mat3 k13;
        phyanim::Mat3 k23;
    } KMat;

    typedef std::vector<KMat> KMats;

    void _computeTet(TetPtr tet, KMat& kMat, Eigen::MatrixXd D);

    void _buildKTriplets(Tets tets,
                         KMats& kMats,
                         double dt2,
                         Triplets& kTriplets,
                         Triplets& aTriplets);

    void _addMatrixToTriplets(uint64_t id0,
                              uint64_t id1,
                              const phyanim::Mat3& m,
                              Triplets& triplets);

    void _addIdentityValueToTriplets(uint64_t id0,
                                     double value,
                                     Triplets& triplets);
};

}  // namespace examples

#endif
