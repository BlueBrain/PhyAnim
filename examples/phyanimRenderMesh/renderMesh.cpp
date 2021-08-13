#include <GLFWApp.h>

int main(int argc, char* argv[])
{
    examples::GLFWApp app;
    app.init(argc, argv);
    app.loop();

    return 0;
}
