#ifndef __PHYANIM_IMPLICITFEMSYSTEM__
#define __PHYANIM_IMPLICITFEMSYSTEM__

#include "AnimSystem.h"

namespace phyanim
{
typedef Eigen::Triplet<double> Triplet;
typedef std::vector<Triplet> Triplets;

class ImplicitFEMSystem : public AnimSystem
{
public:
    ImplicitFEMSystem(double dt, CollisionDetection* collDetector_ = nullptr);

    virtual ~ImplicitFEMSystem(void);

    void preprocessMesh(Mesh* mesh_);

private:
    typedef struct K
    {
        Mat3 k00;
        Mat3 k11;
        Mat3 k22;
        Mat3 k33;
        Mat3 k01;
        Mat3 k02;
        Mat3 k03;
        Mat3 k12;
        Mat3 k13;
        Mat3 k23;
    } TK;

    typedef std::vector<TK> TKs;

    void _step(Mesh* mesh);

    void _conformKMatrix(Mesh* mesh);

    void _buildKTriplets(const Primitives& tets,
                         TKs& ks,
                         double dt2,
                         Triplets& kTriplets,
                         Triplets& aTriplets);

    void _computeTetsK(const Primitives& tets,
                       TKs& ks,
                       double D0,
                       double D1,
                       double D2);

    void _addMatrixToTriplets(uint64_t id0,
                              uint64_t id1,
                              const Mat3& m,
                              Triplets& triplets,
                              uint64_t tripletId);

    void _addIdentityValueToTriplets(uint64_t id0,
                                     double value,
                                     Triplets& triplets,
                                     uint64_t tripletId);

    void _addVec3ToVecX(uint64_t id, const Vec3& value, Eigen::VectorXd& vecx);
};

}  // namespace phyanim

#endif
