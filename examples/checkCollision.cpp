#include <iostream>

#include <CollisionDetection.h>


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage error:\nUse: " << argv[0] << " file_name file_name" <<
                std::endl;
    }
    phyanim::Mesh* mesh0 = new phyanim::Mesh();
    phyanim::Mesh* mesh1 = new phyanim::Mesh();
    mesh0->load(std::string(argv[1]));
    mesh1->load(std::string(argv[2]));


    phyanim::CollisionDetection collisionDetector;
    phyanim::Meshes meshes;
    meshes.push_back(mesh0);
    meshes.push_back(mesh1);
    collisionDetector.dynamicMeshes(meshes);
    
    if (collisionDetector.update()) {
        std::cout << "Meshes colliding" << std::endl;
    } else {
        std::cout << "Meshes not colliding" << std::endl;
    }
}
