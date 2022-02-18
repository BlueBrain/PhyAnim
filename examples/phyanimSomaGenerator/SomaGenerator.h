#ifndef __EXAMPLES_SOMA_GENERATOR__
#define __EXAMPLES_SOMA_GENERATOR__

#include "Icosphere.h"
#include "Sample.h"
#include "StiffnessMatrix.h"

namespace examples
{
typedef std::pair<phyanim::Vec3, double> NeuriteStart;
typedef std::vector<NeuriteStart> NeuriteStarts;

class SomaGenerator
{
public:
    SomaGenerator(Samples starts,
                  Sample soma,
                  double dt = 0.01,
                  double stiffness = 1000.0,
                  double poissonRatio = 0.49,
                  double alphaSoma = 0.75);

    ~SomaGenerator(){};

    void anim(bool updateNodes = false);

    void pull(float alpha);

    phyanim::DrawableMesh* mesh();

private:
    void _addVec3ToVec(uint64_t id,
                       const phyanim::Vec3& value,
                       Eigen::VectorXd& vec);

    void _computeStartsNodes();

    void _fixCenterNodes(double radialDist = 0.5);

    void _updateNodes();

    Icosphere* _ico;
    phyanim::DrawableMesh* _mesh;
    std::vector<Node*> _nodes;
    Tets _tets;
    StiffnessMatrix _kMat;

    Samples _starts;
    std::vector<phyanim::Vec3> _positions;
    std::vector<Nodes> _startsNodes;
    Sample _soma;
    double _dt;
};

}  // namespace examples

#endif
