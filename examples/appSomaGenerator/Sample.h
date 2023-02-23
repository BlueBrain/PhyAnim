
#ifndef __EXAMPLES_SAMPLES__
#define __EXAMPLES_SAMPLES__

#include <Phyanim.h>

namespace examples
{
typedef enum SampleType
{
    SOMA,
    AXON,
    BASAL,
    APICAL,
    UNKNOWN_SAMPLE

} SampleType;

class Sample
{
public:
    uint64_t id;

    SampleType type;

    std::vector<Sample*> children;

    phyanim::Vec3 position;

    double radius;
};

typedef std::vector<Sample*> Samples;

}  // namespace examples
#endif