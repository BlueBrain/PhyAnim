#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Scene.h"

examples::Camera* camera;
examples::Scene* scene;

void render(void);
void idle(void);
void reshape(int width_, int height_);
void keyboard(unsigned char key_, int x_, int y_);


int main(int argc, char* argv[]){

    std::string usage = std::string("Usage ") + std::string(argv[0]) +
            std::string(" -dt [float] -stiffness [float] --help");
    
    float stiffness = 1000.0f;
    float dt = 1.0f / 60.0f;

    for (int i = 1; i < argc; ++i) {
        std::string option(argv[i]);
        try {
            if (option.compare("-dt") == 0) {
                dt = std::atof(argv[i+1]);
                ++i;
            } else if (option.compare("-stiffness") == 0) {
                stiffness = std::atof(argv[i+1]);
                ++i;
            } else if (option.compare("--help") == 0) {
                std::cout << usage << std::endl;
                return 0;
            }
        } catch (...) {
            std::cerr << "Error: " << usage << std::endl;
            return 1;
        }
    }
    
    glutInit(&argc, argv);
    glutInitContextVersion(4, 0);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("SimVisualizer");

    glewExperimental = GL_TRUE;
    glewInit();

    auto version = glGetString(GL_VERSION);

    std::cout << "Lopaded OpenGL " << version << " version" << std::endl;

    glutDisplayFunc(render);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    camera = new examples::Camera(phyanim::Vec3(0.0f, 3.0f, 7.0f));
    scene = new examples::Scene(camera, dt, stiffness);

    glutMainLoop();

    return 0;
}


void render(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->render();
    glutSwapBuffers();
}

void idle(void){
    glutPostRedisplay();
}

void reshape(int width_, int height_){
    camera->ratio(1.0f*width_/height_);
    glViewport(0, 0, width_, height_);
}


void keyboard(unsigned char key_, int x_, int y_)
{
    float dx = 0.1f;
    phyanim::Vec3 dxyz(0.0f, 0.0f, 0.0f);
    bool cameraDisplaced = false;
    switch(key_){
    case 'w':
        dxyz = phyanim::Vec3(0.0f, 0.0f, -dx);
        cameraDisplaced = true;
        break;
    case 's':
        dxyz = phyanim::Vec3(0.0f, 0.0f, dx);
        cameraDisplaced = true;
        break;
    case 'd':
        dxyz = phyanim::Vec3(dx, 0.0f, 0.0f);
        cameraDisplaced = true;
        break;
    case 'a':
        dxyz = phyanim::Vec3(-dx, 0.0f, 0.0f);
        cameraDisplaced = true;
        break;
    case 'r':
        dxyz = phyanim::Vec3(0.0f, dx, 0.0f);
        cameraDisplaced = true;
        break;
    case 'f':
        dxyz = phyanim::Vec3(0.0f, -dx, 0.0f);
        cameraDisplaced = true;
        break;
    case 'z':
        std::cout << "Gravity changed" << std::endl;
        scene->gravity();
        break;
    case 'x':
        std::cout << "Floor collision changed" << std::endl;
        scene->floorCollision();
        break;
    case 'q':
        scene->restart();
        break;
    }
    if (cameraDisplaced) {
        auto pos = camera->position() + dxyz;
        camera->position(pos);
        std::cout << "Camera position: " << pos.x() << " " << pos.y() << " "
                  << pos.z() << std::endl;
    }   
}

