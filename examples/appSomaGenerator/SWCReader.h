#ifndef __EXAMPLES_SEC_READER__
#define __EXAMPLES_SEC_READER__

#include "Sample.h"

namespace examples
{
class SWCReader
{
public:
    SWCReader(const std::string& filePath);

private:
    void _readSamples(const std::string& filePath);

    void _computeSoma();

public:
    Sample soma;

    Samples neurites;

private:
    Samples _somaSamples;
};

}  // namespace examples
#endif  // ULTRALISER_DATA_MORPHOLOGIES_NEURON_SWC_READER_H
