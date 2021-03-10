#include <iostream>

#include <Mesh.h>

int main(int argc, char* argv[])
{
    phyanim::Mesh* mesh;
    for (unsigned int i=1; i < argc; ++i)
    {
        std::string nodesFile(argv[i]);
        ++i;
        std::string elesFile(argv[i]);

        std::string outFile = nodesFile.substr(0, nodesFile.find(".node"));
        outFile.append(".obj");
        mesh = new phyanim::Mesh();
        mesh->load(nodesFile, elesFile);
        mesh->write(outFile);
        std::cout << "Mesh written to file: " << outFile<< std::endl;
    }
    
    return 0;
}
