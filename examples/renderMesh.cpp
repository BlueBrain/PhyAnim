#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <igl/readOFF.h>

#include "Scene.h"

examples::Scene* scene;
examples::Camera* camera;
std::vector<std::string> mesh_files;

int main(int argc, char* argv[])
{
    std::string usage = std::string("Usage error: Use ") + std::string(argv[0]) +
            std::string(" mesh_file[.obj|.off]");
    if (argc < 2)
    {
        std::cerr << usage << std::endl;
        return -1;
    }
    for (uint32_t i = 1; i < argc; ++i)
    {
        std::cout << i << std::endl;
        std::cout << argv[i] << std::endl;
        mesh_files.push_back(std::string(argv[i]));
    }
    std::cout << "Number of files  " << mesh_files.size() << std::endl;
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); 
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    window = glfwCreateWindow(600,600,"Mesh", NULL, NULL);

    if (!window) {
        std::cout << "Not window created" << std::endl;
        glfwTerminate();
        return -1;
    }

    // glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    auto version = glGetString(GL_VERSION);
    
    std::cout << "Lopaded OpenGL " << version << " version" << std::endl;

    camera = new examples::Camera();
    scene = new examples::Scene(camera);

    phyanim::DrawableMesh* mesh;

    phyanim::AABB limits;
    
    for (auto file: mesh_files)
    {
        mesh = new phyanim::DrawableMesh();
        std::cout << "Loading file " << file << std::endl;
        mesh->load(file);
        mesh->upload();
        scene->addMesh(mesh);
        limits.update(mesh->aabb->root->aabb);
    }

    phyanim::Vec3 cameraPos = limits.center();
    std::cout << cameraPos << std::endl;
    cameraPos.z() += (limits.upperLimit.z() - cameraPos.z()) * 2;
    camera->position(cameraPos);

    std::cout << "camera position: "<< camera->position() << std::endl;
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        scene->render();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
