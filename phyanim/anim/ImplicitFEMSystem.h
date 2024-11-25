/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
