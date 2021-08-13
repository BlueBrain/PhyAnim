#include "OverlapApp.h"

int main(int argc, char* argv[])
{
    examples::OverlapApp app;

    app.init(argc, argv);

    app.loop();

    return 0;
}
