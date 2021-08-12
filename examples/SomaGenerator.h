#ifndef __EXAMPLES_SOMA_GENERATOR__
#define __EXAMPLES_SOMA_GENERATOR__

#include "Icosphere.h"

namespace examples
{
typedef std::pair<phyanim::Vec3, double> NeuriteStart;
typedef std::vector<NeuriteStart> NeuriteStarts;

class SomaGenerator
{
public:
    SomaGenerator(NeuriteStarts starts,
                  phyanim::Vec3 pos = phyanim::Vec3::Zero(),
                  double radius = 1.0,
                  double dt = 0.01,
                  double stiffness = 1.0,
                  double fixedThreshold = 0.5);

    ~SomaGenerator(){};

    void simulate(uint64_t iters = 1000);

    void anim(bool updateNodes = false);

    phyanim::DrawableMesh* mesh();

private:
    void _pullSprings(NeuriteStarts starts,
                      double stiffness,
                      double resThreshold = 0.1);

    void _innerSprings(double stiffness);

    void _fixCenterNodes(double threshold = 0.7);

    void _updateNodes();

    Icosphere* _ico;
    phyanim::DrawableMesh* _mesh;
    std::vector<Node*> _nodes;
    std::vector<Node*> _pullNodes;
    Node* _centerNode;
    std::vector<Spring*> _springs;

    phyanim::Vec3 _center;
    double _radius;
    double _dt;
    double _stiffness;
    double _damping;
};

}  // namespace examples

#endif
