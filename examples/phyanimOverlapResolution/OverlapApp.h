#ifndef __EXAMPLES_OVERLAP_APP__
#define __EXAMPLES_OVERLAP_APP__

#include <GLFWApp.h>

#include <chrono>

namespace examples
{
class OverlapApp : public GLFWApp
{
public:
    OverlapApp(int argc, char** argv);

protected:
    void _actionLoop();
};

}  // namespace examples

#endif
