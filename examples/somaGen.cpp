#include "SomaApp.h"

int main(int argc, char* argv[])
{
    examples::SomaApp app;
    app.init(argc, argv);
    app.loop();

    return 0;
}
