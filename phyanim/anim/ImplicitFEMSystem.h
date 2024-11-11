/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __PHYANIM_IMPLICITFEMSYSTEM__
#define __PHYANIM_IMPLICITFEMSYSTEM__

#include "AnimSystem.h"

namespace phyanim
{
namespace anim
{
typedef Eigen::Triplet<float> Triplet;
typedef std::vector<Triplet> Triplets;

class ImplicitFEMSystem : public AnimSystem
{
public:
    ImplicitFEMSystem(float dt, CollisionDetection* collDetector_ = nullptr);

    virtual ~ImplicitFEMSystem(void);

    void preprocessMesh(geometry::MeshPtr mesh_);

private:
    typedef struct K
    {
        Eigen::Matrix3f k00;
        Eigen::Matrix3f k11;
        Eigen::Matrix3f k22;
        Eigen::Matrix3f k33;
        Eigen::Matrix3f k01;
        Eigen::Matrix3f k02;
        Eigen::Matrix3f k03;
        Eigen::Matrix3f k12;
        Eigen::Matrix3f k13;
        Eigen::Matrix3f k23;
    } TK;

    typedef std::vector<TK> TKs;

    void _step(geometry::MeshPtr mesh);

    void _conformKMatrix(geometry::MeshPtr mesh);

    void _buildKTriplets(const geometry::Primitives& tets,
                         TKs& ks,
                         float dt2,
                         Triplets& kTriplets,
                         Triplets& aTriplets);

    void _computeTetsK(const geometry::Primitives& tets,
                       TKs& ks,
                       float D0,
                       float D1,
                       float D2);

    void _addMatrixToTriplets(uint64_t id0,
                              uint64_t id1,
                              const Eigen::Matrix3f& m,
                              Triplets& triplets,
                              uint64_t tripletId);

    void _addIdentityValueToTriplets(uint64_t id0,
                                     float value,
                                     Triplets& triplets,
                                     uint64_t tripletId);

    void _addVec3ToVecX(uint64_t id,
                        const geometry::Vec3& value,
                        Eigen::VectorXf& vecx);
};

}  // namespace anim
}  // namespace phyanim

#endif
