#include "MoveMeshApp.h"

int main(int argc, char* argv[])
{
    examples::MoveMeshApp app;
    app.init(argc, argv);

    app.loop();

    return 0;
}
