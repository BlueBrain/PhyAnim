#ifndef __PHYANIM_IMPLICITFEMSYSTEM__
#define __PHYANIM_IMPLICITFEMSYSTEM__

#include <AnimSystem.h>

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
    void _step(Mesh* mesh);

    void _conformKMatrix(Mesh* mesh);

    void _buildKTriplets(const Tetrahedra& tets,
                         double dt2,
                         Triplets& kTriplets,
                         Triplets& aTriplets);

    void _computeTetsK(const Tetrahedra& tets, double D0, double D1, double D2);

    Mat3 _buildTetK(Vec3 bn,
                    Vec3 bm,
                    double D0,
                    double D1,
                    double D2,
                    double volume);

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
