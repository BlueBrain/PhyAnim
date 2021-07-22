#include "AnimMeshApp.h"

int main(int argc, char* argv[])
{
    examples::AnimMeshApp app;
    app.init(argc, argv);

    app.loop();

    return 0;
}
