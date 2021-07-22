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
                  double radius = 1.0f,
                  double outterStiff = 1000.0,
                  double innerStiff = 100.0,
                  double pullStiff = 100.0,
                  double damping = 0.2,
                  double fixedThreshold = 0.8,
                  double pullResThreshold = 0.2);

    ~SomaGenerator(){};

    void simulate(double dt = 0.01, uint64_t iters = 1000);

    void anim(double dt = 0.01, bool updateNodes = false);

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

    double _damping;
};

}  // namespace examples

#endif
