#include <iostream>

#include <Mesh.h>



bool equivalentMeshes(phyanim::Mesh* mesh0_, phyanim::Mesh* mesh1_);

void message(bool right_) {
    if (right_) {
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "failed" << std::endl;
    }
}

int main(int argc, char** argv) {
    std::cout << "Mesh test" << std::endl;

    std::string file0;
    std::string file1;
    if (argc>1)
    {
        file0 = std::string(argv[1]);
    }
    if (argc>2)
    {
        file1 = std::string(argv[2]);
    }
    
    auto node0 = new phyanim::Node(phyanim::Vec3(0.0, 1.633, 0.0), 0);
    auto node1 = new phyanim::Node(phyanim::Vec3(0.0, 0.0, 1.14315), 1);
    auto node2 = new phyanim::Node(phyanim::Vec3(1.0, 0.0, -0.571577), 2);
    auto node3 = new phyanim::Node(phyanim::Vec3(-1.0, 0.0, -0.571577), 3);
    
    auto tetrahedron = new phyanim::Tetrahedron(node0, node1, node2, node3);

    auto mesh = new phyanim::Mesh();
    mesh->tetrahedra.push_back(tetrahedron);
    mesh->tetsToNodes();
    mesh->tetsToTriangles();
    mesh->tetsToEdges();;
    

    std::cout << "Check right copy: ";
    phyanim::Mesh* meshCopied = mesh->copy();
    message(equivalentMeshes(mesh, meshCopied));
    
    std::cout << "Check different nodes: ";
    delete meshCopied;
    meshCopied = mesh->copy();
    meshCopied->nodes[0]->position.x() = 1.0;
    message(!equivalentMeshes(mesh, meshCopied));

    std::cout << "Check different triangles: ";
    delete meshCopied;
    meshCopied = mesh->copy();
    auto triangle = meshCopied->triangles[0];
    std::swap(triangle->node1, triangle->node2);
    message(!equivalentMeshes(mesh, meshCopied));

    std::cout << "Check different surface triangles: ";
    delete meshCopied;
    meshCopied = mesh->copy();
    auto surfaceTriangle = meshCopied->surfaceTriangles[0];
    std::swap(surfaceTriangle->node1, surfaceTriangle->node2);
    message(!equivalentMeshes(mesh, meshCopied));

    std::cout << "Check different tetrahedra: ";
    delete meshCopied;
    meshCopied = mesh->copy();
    auto tet = meshCopied->tetrahedra[0];
    std::swap(tet->node2, tet->node3);
    message(!equivalentMeshes(mesh, meshCopied));

    std::cout << "Check different edges: ";
    delete meshCopied;
    meshCopied = mesh->copy();
    auto edge = meshCopied->edges[0];
    std::swap(edge->node0, edge->node1);
    message(!equivalentMeshes(mesh, meshCopied));


    mesh = new phyanim::Mesh();

    if (!file0.empty())
    {
        if (!file1.empty())
        {
            mesh->load(file0, file1);
        }
        else
        {
            mesh->load(file0);
        }
    }

    for (auto node: mesh->nodes)
    {
        std::cout << "node " << node->id << ": " << node->position.x() << " " <<
                node->position.y() << " " <<  node->position.z() << std::endl;
    }
    for (auto tet: mesh->tetrahedra)
    {
        std::cout << "tet: " << tet->node0->id << " " << tet->node1->id <<
                " " << tet->node2->id << " " << tet->node3->id << std::endl;
    }
}


bool equivalentMeshes(phyanim::Mesh* mesh0_, phyanim::Mesh* mesh1_) {
    
    bool rightCopy = (mesh0_->nodes.size() == mesh1_->nodes.size()) &&
            (mesh0_->triangles.size() == mesh1_->triangles.size()) &&
            (mesh0_->surfaceTriangles.size() ==
             mesh1_->surfaceTriangles.size()) &&
            (mesh0_->tetrahedra.size() == mesh1_->tetrahedra.size()) &&
            (mesh0_->edges.size() == mesh1_->edges.size());

    if (rightCopy) {
        for (unsigned int i=0; i<mesh0_->nodes.size(); i++) {
            if (*mesh0_->nodes[i] != *mesh1_->nodes[i]) {
                rightCopy = false;
                break;
            }
        }
        for (unsigned int i=0; i<mesh0_->triangles.size(); i++) {
            auto triangle0 = mesh0_->triangles[i];
            auto triangle1 = mesh1_->triangles[i];
            if (*triangle0->node0 != *triangle1->node0 ||
                *triangle0->node1 != *triangle1->node1 ||
                *triangle0->node2 != *triangle1->node2) {
                rightCopy = false;
                break;
            }
        }
        for (unsigned int i=0; i<mesh0_->surfaceTriangles.size(); i++) {
            auto triangle0 = mesh0_->surfaceTriangles[i];
            auto triangle1 = mesh1_->surfaceTriangles[i];
            if (*triangle0->node0 != *triangle1->node0 ||
                *triangle0->node1 != *triangle1->node1 ||
                *triangle0->node2 != *triangle1->node2) {
                rightCopy = false;
                break;
            }
        }
        for (unsigned int i=0; i<mesh0_->tetrahedra.size(); i++) {
            auto tet0 = mesh0_->tetrahedra[i];
            auto tet1 = mesh1_->tetrahedra[i];
            if (*tet0->node0 != *tet1->node0 ||
                *tet0->node1 != *tet1->node1 ||
                *tet0->node2 != *tet1->node2 ||
                *tet0->node3 != *tet1->node3) {
                rightCopy = false;
                break;
            }
        }
        for (unsigned int i=0; i<mesh0_->edges.size(); i++) {
            auto edge0 = mesh0_->edges[i];
            auto edge1 = mesh1_->edges[i];
            if (*edge0->node0 != *edge1->node0 ||
                *edge0->node1 != *edge1->node1) {
                rightCopy = false;
                break;
            }
        }
    }

    return rightCopy;

}
