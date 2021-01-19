#include <iostream>

#include <CollisionDetection.h>

namespace phyanim {

CollisionDetection::CollisionDetection(double stiffness_)
    : aabb(Vec3(-100.0, -100.0, -100.0), Vec3(100.0, 100.0, 100.0)),
      stiffness(stiffness_) {
#ifdef PHYANIM_USES_OPENMP
    _writelock = new omp_lock_t;
    omp_init_lock(_writelock);
#endif
}

CollisionDetection::~CollisionDetection() {
#ifdef PHYANIM_USES_OPENMP
    omp_destroy_lock(_writelock);
    delete _writelock;
#endif
}

void CollisionDetection::dynamicMeshes(Meshes meshes_) {
    _dynamicMeshes = meshes_;
}

void CollisionDetection::staticMeshes(Meshes meshes_) {
    _staticMeshes = meshes_;
}

void CollisionDetection::clear() {
    _dynamicMeshes.clear();
    _staticMeshes.clear();
}

bool CollisionDetection::update() {
    unsigned int size = _dynamicMeshes.size();
    bool detectedCollision = false;
    for (unsigned int i=0; i<size; ++i) {
        auto mesh0 = _dynamicMeshes[i];
        for (unsigned int j=i+1; j<size; ++j) {
            auto mesh1 = _dynamicMeshes[j];
            detectedCollision |= _checkMeshesCollision(mesh0, mesh1);
        }
        for (unsigned int j=0; j<_staticMeshes.size(); j++) {
            auto mesh1 = _staticMeshes[j];
            detectedCollision |= _checkMeshesCollision(mesh0, mesh1);
        }
    }
    return detectedCollision;
}

void CollisionDetection::checkLimitsCollision(void) {
    for (auto mesh:_dynamicMeshes) {
        auto nodes = mesh->aabb->outterNodes(aabb);
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i=0; i<nodes.size(); i++) {
            auto node = nodes[i];
            bool collision = false;
            Vec3 pos = node->position;
            if (pos.x() <= aabb.lowerLimit.x()) {
                pos.x() = aabb.lowerLimit.x();
                collision = true;
            } else if (pos.x() >= aabb.upperLimit.x()) {
                pos.x() = aabb.upperLimit.x();
                collision = true;
            }
            if (pos.y() <= aabb.lowerLimit.y()) {
                pos.y() = aabb.lowerLimit.y();
                collision = true;
            } else if (pos.y() >= aabb.upperLimit.y()) {
                    pos.y() = aabb.upperLimit.y();
                    collision = true;
            }
            if (pos.z() <= aabb.lowerLimit.z()) {
                pos.z() = aabb.lowerLimit.z();
                collision = true;
                 } else if (pos.z() >= aabb.upperLimit.z()) {
                pos.z() = aabb.upperLimit.z();
                collision = true;
            }
            
            if (collision) {
                node->position = pos;
                node->velocity = Vec3::Zero();
            }
        }
    }
}

bool CollisionDetection::_checkMeshesCollision(Mesh* m0_, Mesh* m1_) {
    bool detectedCollision = false;
    auto aabb0 = m0_->aabb;
    auto aabb1 = m1_->aabb;
    auto trianglePairs = aabb0->trianglePairs(aabb1);
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (unsigned int i=0; i<trianglePairs.size(); i++) {
        auto tPair = trianglePairs[i];
        bool collision = _checkTrianglesCollision(tPair.first, tPair.second);
#ifdef PHYANIM_USES_OPENMP
        omp_set_lock(_writelock);
#endif
        detectedCollision |= collision;
#ifdef PHYANIM_USES_OPENMP
        omp_unset_lock(_writelock);
#endif
    }
    return detectedCollision;
}

bool CollisionDetection::_checkTrianglesCollision(Triangle* t0_, Triangle* t1_) {
    Vec3 vt0[3], vt1[3];
    vt0[0] = t0_->node0->position;
    vt0[1] = t0_->node1->position;
    vt0[2] = t0_->node2->position;
    vt1[0] = t1_->node0->position;
    vt1[1] = t1_->node1->position;
    vt1[2] = t1_->node2->position;

    Vec3 n0 = (vt0[1]-vt0[0]).cross(vt0[2]-vt0[0]);
    double d0 = -n0.dot(vt0[0]);

    double dt1[3];
    dt1[0] = n0.dot(vt1[0])+d0;
    dt1[1] = n0.dot(vt1[1])+d0;
    dt1[2] = n0.dot(vt1[2])+d0;

    if ((dt1[0]>0.0 && dt1[1]>0.0 && dt1[2]>0.0) ||
        (dt1[0]<0.0 && dt1[1]<0.0 && dt1[2]<0.0))
    {
        return false;
    }

    Vec3 n1 = (vt1[1]-vt1[0]).cross(vt1[2]-vt1[0]);
    double d1 = -n1.dot(vt1[0]);

    double dt0[3];
    dt0[0] = n1.dot(vt0[0])+d1;
    dt0[1] = n1.dot(vt0[1])+d1;
    dt0[2] = n1.dot(vt0[2])+d1;

    
    if ((dt0[0]>0.0 && dt0[1]>0.0 && dt0[2]>0.0) ||
        (dt0[0]<0.0 && dt0[1]<0.0 && dt0[2]<0.0))
    {
        return false;
    }
    
    Vec3 D = n0.cross(n1);

    double pt0[3], pt1[3];
    pt0[0] = D.dot(vt0[0]);
    pt0[1] = D.dot(vt0[1]);
    pt0[2] = D.dot(vt0[2]);
    pt1[0] = D.dot(vt1[0]);
    pt1[1] = D.dot(vt1[1]);
    pt1[2] = D.dot(vt1[2]);

    double sdt0[3], sdt1[3];
    sdt0[0] = (dt0[0]>0) - (dt0[0]<0);
    sdt0[1] = (dt0[1]>0) - (dt0[1]<0);
    sdt0[2] = (dt0[2]>0) - (dt0[2]<0);
    sdt1[0] = (dt1[0]>0) - (dt1[0]<0);
    sdt1[1] = (dt1[1]>0) - (dt1[1]<0);
    sdt1[2] = (dt1[2]>0) - (dt1[2]<0);

    unsigned int offt0 = 0;
    if (sdt0[1] != sdt0[2]) {
        if (sdt0[0] == sdt0[2]) {
            offt0 = 1;
        } else {
            offt0 = 2;
        }
    }

    unsigned int offt1 = 0;
    if (sdt1[1] != sdt1[2]) {
        if (sdt1[0] == sdt1[2]) {
            offt1 = 1;
        } else {
            offt1 = 2;
        }
    }

    double interval0[2], interval1[2];

    unsigned int t0id0 = offt0;
    unsigned int t0id1 = (offt0+1)%3;
    unsigned int t0id2 = (offt0+2)%3;

    unsigned int t1id0 = offt1;
    unsigned int t1id1 = (offt1+1)%3;
    unsigned int t1id2 = (offt1+2)%3;
    
    interval0[0] = pt0[t0id1] + (pt0[t0id0]-pt0[t0id1]) * dt0[t0id1] /
            (dt0[t0id1]-dt0[t0id0]);
    interval0[1] = pt0[t0id2] + (pt0[t0id0]-pt0[t0id2]) * dt0[t0id2] /
            (dt0[t0id2]-dt0[t0id0]);    

    
    interval1[0] = pt1[t1id1] + (pt1[t1id0]-pt1[t1id1]) * dt1[t1id1] /
            (dt1[t1id1]-dt1[t1id0]);
    interval1[1] = pt1[t1id2] + (pt1[t1id0]-pt1[t1id2]) * dt1[t1id2] /
            (dt1[t1id2]-dt1[t1id0]);    
    
    if (interval0[0] > interval0[1]) {
        std::swap(interval0[0],interval0[1]);
    }
    if (interval1[0] > interval1[1]) {
        std::swap(interval1[0],interval1[1]);
    }
    if (interval0[1] < interval1[0]) {
        return false;
    }
    if (interval1[1] < interval0[0]) {
        return false;
    }

    n0.normalize();
    n1.normalize();
    _checkAndSetForce(t0_->node0, n1, dt0[0]);
    _checkAndSetForce(t0_->node1, n1, dt0[1]);
    _checkAndSetForce(t0_->node2, n1, dt0[2]);
    _checkAndSetForce(t1_->node0, n0, dt1[0]);
    _checkAndSetForce(t1_->node1, n0, dt1[1]);
    _checkAndSetForce(t1_->node2, n0, dt1[2]);
    
    return true;
}

void CollisionDetection::_checkAndSetForce(Node* node_, Vec3 normal_,
                                           double dist_) {
    if (dist_<0.0) {
        node_->force += -dist_*stiffness*normal_;
    }
}

}
