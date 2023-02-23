#include "SWCReader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace examples
{
SWCReader::SWCReader(const std::string& filePath)
{
    _readSamples(filePath);
    _computeSoma();
}

void SWCReader::_readSamples(const std::string& filePath)
{
    std::unordered_map<uint64_t, Sample*> samplesMap;
    neurites.clear();
    try
    {
        std::ifstream swcFile(filePath.c_str());
        if (!swcFile.is_open())
        {
            std::cerr << "Unable to open an SWC morphology file [ "
                      << filePath.c_str() << " ]" << std::endl;
        }
        std::string line;
        while (!swcFile.eof())
        {
            std::getline(swcFile, line);

            // Remove whitespaces at the beginning of the line
            line.erase(0, line.find_first_not_of(" "));
            if (line.empty() || line[0] == '#') continue;

            Sample* sample = new Sample();
            int64_t parentId, type;
            std::stringstream sstr(line);
            sstr >> sample->id >> type >> sample->position.x() >>
                sample->position.y() >> sample->position.z() >>
                sample->radius >> parentId;

            switch (type)
            {
            case 1:
                sample->type = SampleType::SOMA;
                break;
            case 2:
                sample->type = SampleType::AXON;
                break;
            case 3:
                sample->type = SampleType::BASAL;
                break;
            case 4:
                sample->type = SampleType::APICAL;
                break;
            default:
                sample->type = UNKNOWN_SAMPLE;
                break;
            }

            samplesMap[sample->id] = sample;

            if (sample->type == SampleType::SOMA)
                _somaSamples.push_back(sample);
            else if (parentId >= 0)
            {
                auto search = samplesMap.find(parentId);
                if (search != samplesMap.end())
                {
                    auto parent = search->second;
                    if (parent->type == SampleType::SOMA)
                        neurites.push_back(sample);
                }
            }
        }
    }
    catch (const std::exception&)
    {
        std::cerr << "Unable to load an SWC morphology file [ "
                  << filePath.c_str() << " ]" << std::endl;
    }
}

void SWCReader::_computeSoma()
{
    soma.position = phyanim::Vec3::Zero();
    soma.radius = 0.0;
    for (auto sample : neurites) soma.position += sample->position;
    if (!neurites.empty())
    {
        soma.position /= neurites.size();

        if (neurites.size() == 1)
            soma.radius = neurites[0]->radius;
        else
        {
            soma.radius = std::numeric_limits<double>::max();
            for (auto sample : neurites)
            {
                double radius = (soma.position - sample->position).norm();
                soma.radius = std::min(soma.radius, radius);
            }
            if (soma.radius <= 0.000001) soma.radius = neurites[0]->radius;
        }
    }
}

}  // namespace examples
