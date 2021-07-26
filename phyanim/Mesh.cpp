#include <BoundingSphere.h>
#include <Mesh.h>
#include <igl/readOBJ.h>
#include <igl/readOFF.h>
#include <igl/readPLY.h>
#include <igl/writeOFF.h>

#include <fstream>
#include <iostream>
#include <set>

namespace phyanim
{
Mesh::Mesh(double stiffness_,
           double density_,
           double damping_,
           double poissonRatio_)
    : initArea(0.0)
    , initVolume(0.0)
    , stiffness(stiffness_)
    , density(density_)
    , damping(damping_)
    , poissonRatio(poissonRatio_)
{
    aabb = new AxisAlignedBoundingBox();
}

Mesh::~Mesh(void) {}

void Mesh::load(const std::string& file_, bool createEdges)
{
    bool tetraLoaded = false;
    if (file_.empty())
    {
        std::cerr << "Error loading: empty file" << std::endl;
        return;
    }
    if (file_.find(".off") != std::string::npos)
    {
        _loadOFF(file_);
    }
    else if (file_.find(".ply") != std::string::npos)
    {
        _loadPLY(file_);
    }
    else if (file_.find(".obj") != std::string::npos)
    {
        _loadOBJ(file_);
    }
    else if (file_.find(".tet") != std::string::npos)
    {
        tetraLoaded = true;
        _loadTET(file_);
    }
    else
    {
        std::cerr << "Error loading file: " << file_ << ". Format unknown"
                  << std::endl;
    }

    if (tetraLoaded)
    {
        tetsToTriangles();
        initVolume = volume();
    }
    else
    {
        surfaceTriangles = triangles;
    }
    if (createEdges)
    {
        trianglesToEdges();
    }
    initArea = area();
    computePerNodeMass();
    aabb->generate(nodes, surfaceTriangles, tetrahedra);
}

void Mesh::load(const std::string& nodeFile_,
                const std::string& eleFile_,
                bool createEdges)
{
    if (nodeFile_.empty() || eleFile_.empty())
    {
        std::cerr << "Error loading: empty file" << std::endl;
        return;
    }
    _loadTETGEN(nodeFile_, eleFile_);
    std::cout << "File load with " << tetrahedra.size() << " tetrahera and "
              << nodes.size() << " vertices" << std::endl;
    tetsToTriangles();
    initVolume = volume();
    if (createEdges)
    {
        trianglesToEdges();
    }
    initArea = area();
    computePerNodeMass();
    aabb->generate(nodes, surfaceTriangles, tetrahedra);
}

void Mesh::write(const std::string& file_)
{
    if (file_.empty())
    {
        std::cerr << "Error writing: empty file"
                  << "hola" << std::endl;
        return;
    }
    if (file_.find(".off") != std::string::npos)
    {
        _writeOFF(file_);
    }
    else if (file_.find(".tet") != std::string::npos)
    {
        _writeTET(file_);
    }
    else
    {
        std::cerr << "Error writing file: " << file_ << ". Format unknown"
                  << std::endl;
    }
}

double Mesh::volume()
{
    double volume = 0;

    if (!tetrahedra.empty())
    {
        for (auto tet : tetrahedra)
        {
            volume += tet->volume();
        }
    }
    return volume;
}

double Mesh::area()
{
    double sumArea = 0.0;
    if (surfaceTriangles.size() > 0)
    {
        for (auto tri : surfaceTriangles)
        {
            sumArea += tri->area();
        }
    }
    else
    {
        for (auto tri : triangles)
        {
            sumArea += tri->area();
        }
    }
    return sumArea;
}

void Mesh::trianglesToNodes()
{
    std::set<Node*> uNodes;
    for (auto tri : triangles)
    {
        uNodes.insert(tri->node0);
        uNodes.insert(tri->node1);
        uNodes.insert(tri->node2);
    }
    nodes.clear();
    nodes.insert(nodes.end(), uNodes.begin(), uNodes.end());
}

void Mesh::trianglesToEdges()
{
    for (auto tri : triangles)
    {
        auto triEdges = tri->edges();
        edges.insert(edges.end(), triEdges.begin(), triEdges.end());
    }
}

void Mesh::tetsToNodes()
{
    std::set<Node*> uNodes;
    for (auto tet : tetrahedra)
    {
        uNodes.insert(tet->node0);
        uNodes.insert(tet->node1);
        uNodes.insert(tet->node2);
        uNodes.insert(tet->node3);
    }
    nodes.clear();
    nodes.insert(nodes.end(), uNodes.begin(), uNodes.end());
}

void Mesh::tetsToEdges()
{
    UniqueEdges uniqueEdges;
    for (auto tet : tetrahedra)
    {
        for (auto edge : tet->edges())
        {
            uniqueEdges.insert(edge);
        }
    }
    edges.insert(edges.end(), uniqueEdges.begin(), uniqueEdges.end());
}

void Mesh::tetsToTriangles()
{
    triangles.clear();
    surfaceTriangles.clear();
    UniqueTriangles surfaceTs;
    std::pair<UniqueTriangles::iterator, bool> insertionResult;
    for (auto tet : tetrahedra)
    {
        for (auto t : tet->triangles())
        {
            insertionResult = surfaceTs.insert(t);
            if (!insertionResult.second)
            {
                surfaceTs.erase(insertionResult.first);
                triangles.push_back(t);
            }
        }
    }
    triangles.insert(triangles.end(), surfaceTs.begin(), surfaceTs.end());
    surfaceTriangles.insert(surfaceTriangles.end(), surfaceTs.begin(),
                            surfaceTs.end());
}

void Mesh::computePerNodeMass(void)
{
    if (tetrahedra.size() > 0)
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i = 0; i < nodes.size(); i++)
        {
            auto node = nodes[i];
            node->mass = 0.0;
        }
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i = 0; i < tetrahedra.size(); i++)
        {
            auto tet = tetrahedra[i];
            double massPerNode = tet->initVolume * density * 0.25;
            tet->node0->mass += massPerNode;
            tet->node1->mass += massPerNode;
            tet->node2->mass += massPerNode;
            tet->node3->mass += massPerNode;
        }
    }
    else
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i = 0; i < nodes.size(); i++)
        {
            auto node = nodes[i];
            node->mass = density;
        }
    }
}

void Mesh::nodesToInitPos()
{
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < nodes.size(); ++i)
    {
        auto node = nodes[i];
        node->position = node->initPosition;
        node->velocity = node->initVelocity;
    }
}

void Mesh::nodesForceZero()
{
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < nodes.size(); ++i)
    {
        auto node = nodes[i];
        node->force = Vec3::Zero();
    }
}

Mesh* Mesh::copy(bool surfaceTriangles_,
                 bool triangles_,
                 bool tetrahedra_,
                 bool edges_)
{
    auto mesh = new Mesh();
    std::unordered_map<Node*, Node*> nodesDicc;
    for (auto node : nodes)
    {
        auto newNode = new Node(node->position, node->id);
        mesh->nodes.push_back(newNode);
        nodesDicc[node] = newNode;
    }
    if (surfaceTriangles_)
    {
        for (auto triangle : surfaceTriangles)
        {
            auto newTriangle = new Triangle(nodesDicc[triangle->node0],
                                            nodesDicc[triangle->node1],
                                            nodesDicc[triangle->node2]);
            mesh->surfaceTriangles.push_back(newTriangle);
        }
    }
    if (triangles_)
    {
        for (auto triangle : triangles)
        {
            auto newTriangle = new Triangle(nodesDicc[triangle->node0],
                                            nodesDicc[triangle->node1],
                                            nodesDicc[triangle->node2]);
            mesh->triangles.push_back(newTriangle);
        }
    }
    if (tetrahedra_)
    {
        for (auto tet : tetrahedra)
        {
            auto newTet =
                new Tetrahedron(nodesDicc[tet->node0], nodesDicc[tet->node1],
                                nodesDicc[tet->node2], nodesDicc[tet->node3]);
            mesh->tetrahedra.push_back(newTet);
        }
    }
    if (edges_)
    {
        for (auto edge : edges)
        {
            auto newEdge =
                new Edge(nodesDicc[edge->node0], nodesDicc[edge->node1]);
            mesh->edges.push_back(newEdge);
        }
    }
    return mesh;
}

void Mesh::positionDifference(double& mean_,
                              double& max_,
                              double& min_,
                              double& rms_)
{
    unsigned int size = nodes.size();
    mean_ = 0.0;
    max_ = std::numeric_limits<double>::min();
    min_ = std::numeric_limits<double>::max();
    rms_ = 0.0;
    if (size > 0)
    {
        for (unsigned int i = 0; i < size; i++)
        {
            auto node = nodes[i];
            double diff =
                std::abs((node->position - node->initPosition).norm());
            mean_ += diff;
            max_ = std::max(max_, diff);
            min_ = std::min(min_, diff);
            rms_ += diff * diff;
        }
        mean_ /= size;
    }
}

void Mesh::_split(const std::string& string_,
                  std::vector<std::string>& strings_,
                  char delim_)
{
    std::stringstream ss(string_);
    std::string token;
    strings_.clear();
    while (std::getline(ss, token, delim_))
    {
        strings_.push_back(token);
    }
}

void Mesh::_loadOBJ(const std::string& file_)
{
    Eigen::MatrixXd vertices;
    Eigen::MatrixXi facets;
    igl::readOBJ(file_.c_str(), vertices, facets);
    size_t nVertices = vertices.rows();
    nodes.resize(nVertices);
    for (size_t i = 0; i < nVertices; ++i)
    {
        nodes[i] = new phyanim::Node(Vec3(vertices.row(i)), i);
    }
    size_t nFacets = facets.rows();
    bool quads = facets.cols() == 4;
    triangles.resize(nFacets);
    if (quads)
    {
        triangles.resize(nFacets * 2);
    }
    for (size_t i = 0; i < nFacets; ++i)
    {
        triangles[i] = new phyanim::Triangle(
            nodes[facets(i, 0)], nodes[facets(i, 2)], nodes[facets(i, 1)]);
        if (quads)
        {
            triangles[nFacets + i] = new phyanim::Triangle(
                nodes[facets(i, 0)], nodes[facets(i, 3)], nodes[facets(i, 2)]);
        }
    }
}

void Mesh::_loadOFF(const std::string& file_)
{
    Eigen::MatrixXd vertices;
    Eigen::MatrixXi facets;
    igl::readOFF(file_.c_str(), vertices, facets);
    size_t nVertices = vertices.rows();
    nodes.resize(nVertices);
    for (size_t i = 0; i < nVertices; ++i)
    {
        nodes[i] = new phyanim::Node(Vec3(vertices.row(i)), i);
    }
    size_t nFacets = facets.rows();
    bool quads = facets.cols() == 4;
    triangles.resize(nFacets);
    if (quads)
    {
        triangles.resize(nFacets * 2);
    }
    for (size_t i = 0; i < nFacets; ++i)
    {
        triangles[i] = new phyanim::Triangle(
            nodes[facets(i, 0)], nodes[facets(i, 2)], nodes[facets(i, 1)]);
        if (quads)
        {
            triangles[nFacets + i] = new phyanim::Triangle(
                nodes[facets(i, 0)], nodes[facets(i, 3)], nodes[facets(i, 2)]);
        }
    }
}

void Mesh::_loadPLY(const std::string& file_)
{
    Eigen::MatrixXd vertices;
    Eigen::MatrixXi facets;
    igl::readPLY(file_.c_str(), vertices, facets);
    size_t nVertices = vertices.rows();
    nodes.resize(nVertices);
    for (size_t i = 0; i < nVertices; ++i)
    {
        nodes[i] = new phyanim::Node(Vec3(vertices.row(i)), i);
    }
    size_t nFacets = facets.rows();
    bool quads = facets.cols() == 4;
    triangles.resize(nFacets);
    if (quads)
    {
        triangles.resize(nFacets * 2);
    }
    for (size_t i = 0; i < nFacets; ++i)
    {
        triangles[i] = new phyanim::Triangle(
            nodes[facets(i, 0)], nodes[facets(i, 1)], nodes[facets(i, 2)]);
        if (quads)
        {
            triangles[nFacets + i] = new phyanim::Triangle(
                nodes[facets(i, 0)], nodes[facets(i, 2)], nodes[facets(i, 3)]);
        }
    }
}

void Mesh::_loadTET(const std::string& file_)
{
    std::ifstream inFile(file_.c_str());
    if (inFile.is_open())
    {
        try
        {
            std::string line;
            std::getline(inFile, line);
            std::vector<std::string> strs;
            _split(line, strs);
            unsigned int numVertices = std::atoi(strs[1].c_str());
            unsigned int numTets = std::atoi(strs[2].c_str());

            nodes.resize(numVertices);
            for (unsigned int i = 0; i < numVertices; i++)
            {
                std::getline(inFile, line);
                _split(line, strs);
                Vec3 pos(std::atof(strs[0].c_str()), std::atof(strs[1].c_str()),
                         std::atof(strs[2].c_str()));
                nodes[i] = new phyanim::Node(pos, i);
            }
            tetrahedra.resize(numTets);
            for (unsigned int i = 0; i < numTets; i++)
            {
                std::getline(inFile, line);
                _split(line, strs);
                tetrahedra[i] =
                    new Tetrahedron(nodes[std::atoi(strs[0].c_str())],
                                    nodes[std::atoi(strs[1].c_str())],
                                    nodes[std::atoi(strs[2].c_str())],
                                    nodes[std::atoi(strs[3].c_str())]);
            }
            inFile.close();
        }
        catch (...)
        {
            std::cerr << "Error loading file " << file_ << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: can not open " << file_ << std::endl;
    }
}

void Mesh::_loadTETGEN(const std::string& nodeFile_,
                       const std::string& eleFile_)
{
    try
    {
        std::ifstream nodeFile(nodeFile_.c_str());
        std::string line;
        std::vector<std::string> strs;
        uint64_t count = 0;
        uint64_t dim = 0;
        uint64_t nodePerTet = 0;
        uint64_t attribute = 0;
        uint64_t marker = 0;
        if (nodeFile.is_open())
        {
            while (!nodeFile.eof())
            {
                std::getline(nodeFile, line);
                if (line[0] != '#')
                {
                    std::stringstream sstream(line);
                    if (count == 0)
                    {
                        sstream >> count >> dim >> attribute >> marker;
                        nodes.resize(count);
                    }
                    else
                    {
                        uint64_t index;
                        float xCoord;
                        float yCoord;
                        float zCoord;
                        sstream >> index >> xCoord >> yCoord >> zCoord;
                        Vec3 pos(xCoord, yCoord, zCoord);
                        nodes[index] = new Node(pos, index);
                    }
                }
            }
        }
        else
        {
            std::cerr << "Error: can not open " << nodeFile_ << std::endl;
        }

        count = 0;
        std::ifstream eleFile(eleFile_.c_str());
        if (eleFile.is_open())
        {
            while (!eleFile.eof())
            {
                std::getline(eleFile, line);
                if (line[0] != '#')
                {
                    std::stringstream sstream(line);
                    if (count == 0)
                    {
                        sstream >> count >> nodePerTet >> attribute;
                        tetrahedra.resize(count);
                    }
                    else
                    {
                        uint64_t index;
                        uint64_t id0;
                        uint64_t id1;
                        uint64_t id2;
                        uint64_t id3;
                        sstream >> index >> id0 >> id1 >> id3 >> id2;
                        auto tet = new Tetrahedron(nodes[id0], nodes[id1],
                                                   nodes[id3], nodes[id2]);
                        tetrahedra[index] = tet;
                    }
                }
            }
        }
        else
        {
            std::cerr << "Error: can not open " << eleFile_ << std::endl;
        }
    }
    catch (...)
    {
        std::cerr << "Error while loading files" << std::endl;
    }
}

void Mesh::_writeOFF(const std::string& file_)
{
    unsigned int nVertices = nodes.size();
    unsigned int nFacets = surfaceTriangles.size();
    Eigen::MatrixXd vertices(nVertices, 3);
    Eigen::MatrixXi facets(nFacets, 3);
    for (unsigned int i = 0; i < nVertices; i++)
    {
        nodes[i]->id = i;
        vertices.row(i) = nodes[i]->position;
    }
    for (unsigned int i = 0; i < nFacets; i++)
    {
        Eigen::MatrixXi facet(1, 3);
        facet << surfaceTriangles[i]->node0->id, surfaceTriangles[i]->node1->id,
            surfaceTriangles[i]->node2->id;
        facets.row(i) = facet;
    }
    igl::writeOFF(file_.c_str(), vertices, facets);
}

void Mesh::_writeTET(const std::string& file_)
{
    std::ofstream os(file_.c_str());
    if (!os.is_open())
    {
        return;
    }
    unsigned int nVertices = nodes.size();
    unsigned int nTets = tetrahedra.size();
    os << "tet " << nVertices << " " << nTets << "\n";
    for (unsigned int i = 0; i < nVertices; i++)
    {
        nodes[i]->id = i;
        Vec3 pos = nodes[i]->position;
        os << pos.x() << " " << pos.y() << " " << pos.z() << "\n";
    }
    for (unsigned int i = 0; i < nTets; i++)
    {
        os << tetrahedra[i]->node0->id << " " << tetrahedra[i]->node1->id << " "
           << tetrahedra[i]->node2->id << " " << tetrahedra[i]->node3->id
           << "\n";
    }
    os.close();
}

}  // namespace phyanim
