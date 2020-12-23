#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <igl/readOFF.h>


#include "OverlapScene.h"

examples::Camera* camera;
examples::OverlapScene* scene;

std::vector<std::string> inFiles;


void render(void);

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
                         int mods);

void idle(void);
void reshape(int width_, int height_);


int main(int argc, char* argv[]){

    std::string usage = std::string("SYNOPSIS\n\t") + std::string(argv[0]) +
            std::string(" file [-dt double] [-d double] "
                        "[-ks double] [-kd double] [-kp double] "
                        "[--help]"
                        "\nThe following option is available:"
                        "\n\t-dt double    Set time increment"
                        "\n\t-d double     Set density"
                        "\n\t-ks double    Set stiffness constant/Young modulus"
                        "\n\t-kd double    Set damping constant"
                        "\n\t-kp double    Set poisson ratio"
                        "\n\t--help        Print help message"
                        );
    
    double stiffness = 1000.0;
    double damping = 0.1;
    double density = 10.0;
    double poissonRatio = 0.3;
    double collisionStiffness = 1000.0;
    double dt = 0.01;
    examples::SimSystem simSystem = examples::EXPLICITMASSSPRING;
    if (argc < 2) {
        std::cerr << "Usage error:\n" << usage << std::endl;
        return -1;
    }
    for (int i = 1; i < argc; ++i) {
        std::string option(argv[i]);
        try {
            if (option.compare("-dt") == 0) {
                dt = std::atof(argv[i+1]);
                ++i;
            } else if (option.compare("-ks") == 0) {
                stiffness = std::atof(argv[i+1]);
                ++i;   
            } else if (option.compare("-d") == 0) {
                density = std::atof(argv[i+1]);
                ++i;
            }  else if (option.compare("-kd") == 0) {
                damping = std::atof(argv[i+1]);
                ++i;
            } else if (option.compare("-kp") == 0) {
                poissonRatio = std::atof(argv[i+1]);
                ++i;
            } else if (option.compare("-kc") == 0) {
                collisionStiffness = std::atof(argv[i+1]);
                ++i;
            } else if (option.compare("--help") == 0) {
                std::cout << usage << std::endl;
                return 0;
            } else if (option.compare("-exmass") == 0) {
                simSystem = examples::EXPLICITMASSSPRING;
            } else if (option.compare("-immass") == 0) {
                simSystem = examples::IMPLICITMASSSPRING;
            } else if (option.compare("-exfem") == 0) {
                simSystem = examples::EXPLICITFEM;
            } else if (option.compare("-imfem") == 0) {
                simSystem = examples::IMPLICITFEM;
            }  
            else {
                inFiles.push_back(std::string(argv[i]));
            }
        } catch (...) {
            std::cerr << "Usage error:\n" << usage << std::endl;
            return -1;
        }
    }

    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); 
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    window = glfwCreateWindow(600,600,"Test GLFW", NULL, NULL);

    if (!window) {
        std::cout << "Not windows created" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    auto version = glGetString(GL_VERSION);

    std::cout << "Lopaded OpenGL " << version << " version" << std::endl;

    glfwMakeContextCurrent(window);
    
    camera = new examples::Camera(phyanim::Vec3(0.0, 0.0, 7.0));
    scene = new examples::OverlapScene(inFiles, camera, simSystem, dt, stiffness,
                                       density, damping, poissonRatio,
                                       collisionStiffness);
   
    while(!glfwWindowShouldClose(window)) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
      
    }

    glfwTerminate();
    return 0;
    // glutDisplayFunc(render);
    // glutIdleFunc(idle);
    // glutReshapeFunc(reshape);
    // glutKeyboardFunc(keyboard);

   
    // glutMainLoop();

    return 0;
}


void render(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->render();
}

// void reshape(int width_, int height_){
//     camera->ratio(1.0*width_/height_);
//     glViewport(0, 0, width_, height_);
// }

static void key_callback(GLFWwindow* window_, int key_, int scancode_, int action_, int mods_)
{
    float dx = 0.1f;
    phyanim::Vec3 dxyz(0.0, 0.0, 0.0);
    bool cameraDisplaced = false;

    if (action_ == GLFW_PRESS) {
        switch(key_) {
        case 'V':
            scene->changeRenderMode();
            break;
        }
    }
    switch(key_){
    case 'W':
        dxyz = phyanim::Vec3(0.0, 0.0, -dx);
        cameraDisplaced = true;
        break;
    case 'S':
        dxyz = phyanim::Vec3(0.0, 0.0, dx);
        cameraDisplaced = true;
        break;
    case 'D':
        dxyz = phyanim::Vec3(dx, 0.0, 0.0);
        cameraDisplaced = true;
        break;
    case 'A':
        dxyz = phyanim::Vec3(-dx, 0.0, 0.0);
        cameraDisplaced = true;
        break;
    case 'R':
        dxyz = phyanim::Vec3(0.0, dx, 0.0);
        cameraDisplaced = true;
        break;
    case 'F':
        dxyz = phyanim::Vec3(0.0, -dx, 0.0);
        cameraDisplaced = true;
        break;
    }
    if (cameraDisplaced) {
        auto pos = camera->position() + dxyz;
        camera->position(pos);
        std::cout << "Camera position: " << pos.x() << " " << pos.y() << " "
                  << pos.z() << std::endl;
    }   
}

