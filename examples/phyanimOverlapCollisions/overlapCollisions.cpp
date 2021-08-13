#include "OverlapCollisionsApp.h"

int main(int argc, char* argv[])
{
    examples::OverlapCollisionsApp app;

    app.init(argc, argv);

    app.loop();

    return 0;
}
