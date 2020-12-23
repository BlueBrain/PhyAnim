#ifndef __PHYANIM_MESH__
#define __PHYANIM_MESH__

#include <Tetrahedron.h>
#include <AxisAlignedBoundingBox.h>

namespace phyanim {

class Mesh;

typedef std::vector<Mesh*> Meshes;

class Mesh {

  public:

    Mesh(double stiffness_ = 1000.0, double density_ = 1.0,
         double damping_ = 1.0, double poissonRatio_ = 0.499);

    virtual ~Mesh(void);

    virtual void load(const std::string& file_);

    void write(const std::string& file_);
    
    double volume(void);

    double area(void);

    void trianglesToNodes(void);

    void trianglesToEdges(void);
    
    void tetsToNodes(void);

    void tetsToEdges(void);
    
    void tetsToTriangles(void);

    void nodesToInitPos(void);

    Nodes nodes;

    Edges edges;

    Tetrahedra tetrahedra;
    
    Triangles surfaceTriangles;

    Triangles triangles;

    double initArea;
    
    double initVolume;
    
    double stiffness;

    double density;

    double damping;

    double poissonRatio;

    AxisAlignedBoundingBox* aabb;

  private:

    void _split(const std::string& string_, std::vector<std::string>& strings_, char delim_ = ' ');

    void _loadOBJ(const std::string& file_);

    void _loadOFF(const std::string& file_);

    void _loadPLY(const std::string& file_);

    void _loadTET(const std::string& file_);

    void _writeOFF(const std::string& file_);

    void _writeTET(const std::string& file_);
    
};

}

#endif 
