#ifndef __PHYANIM_IMPLICITMASSSPRINGSYSTEM__
#define __PHYANIM_IMPLICITMASSSPRINGSYSTEM__

#include <AnimSystem.h>

#include <Eigen/Sparse>

namespace phyanim
{
typedef Eigen::Triplet<double> Td;
typedef std::vector<Td> Tds;

class ImplicitMassSpringSystem : public AnimSystem
{
public:
    ImplicitMassSpringSystem(double dt);

    virtual ~ImplicitMassSpringSystem(void);

private:
    void _step(Mesh* mesh);

    Tds _mat3ToTd(Mat3& mat_, unsigned int i_, unsigned int j_);

    void _addBlockToMatrix(Mat3& block_,
                           Eigen::MatrixXd& mat_,
                           unsigned int i_,
                           unsigned int j_);

    Tds _diagVec3ToTd(Vec3& vec_, unsigned int i_, unsigned int j_);

    // Contains the mass coefficients of the A matrices
    std::vector<Tds> _massCoeffs;
};

}  // namespace phyanim

#endif
