#ifndef __PHYANIM_MESH__
#define __PHYANIM_MESH__

#include <HierarchicalAABB.h>
#include <Primitive.h>

#include <Eigen/Sparse>

namespace phyanim
{
class Mesh;

typedef Mesh* MeshPtr;

typedef std::vector<MeshPtr> Meshes;

class Mesh
{
public:
    Mesh(double stiffness_ = 1000.0,
         double density_ = 1.0,
         double damping_ = 1.0,
         double poissonRatio_ = 0.49);

    virtual ~Mesh(void);

    virtual void load(const std::string& file_);

    virtual void load(const std::string& nodeFile_,
                      const std::string& eleFile_);

    void clearData();

    void compute(bool createEdges = false);

    void write(const std::string& file_);

    double volume(void);

    double area(void);

    void trianglesToNodes(void);

    void trianglesToEdges(void);

    void tetsToNodes(void);

    void tetsToEdges(void);

    void tetsToTriangles(void);

    void computePerNodeMass(void);

    void nodesToInitPos(void);

    void nodesForceZero(void);

    virtual Mesh* copy(bool surfaceTriangles_ = true,
                       bool triangles_ = true,
                       bool tetrahedra_ = true,
                       bool edges_ = true);

    void positionDifference(double& mean_,
                            double& max_,
                            double& min_,
                            double& rms_);

    Nodes nodes;

    Primitives surfaceTriangles;

    Primitives triangles;

    Primitives tetrahedra;

    Edges edges;

    HierarchicalAABBPtr boundingBox;

    double initArea;

    double initVolume;

    double stiffness;

    double density;

    double damping;

    double poissonRatio;

    Eigen::SparseMatrix<double> kMatrix;
    Eigen::SparseMatrix<double> AMatrix;
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> AMatrixSolver;

private:
    void _split(const std::string& string_,
                std::vector<std::string>& strings_,
                char delim_ = ' ');

    void _loadOBJ(const std::string& file_);

    void _loadOFF(const std::string& file_);

    void _loadPLY(const std::string& file_);

    void _loadTET(const std::string& file_);

    void _loadTETGEN(const std::string& nodeFile_, const std::string& eleFile_);

    void _writeOFF(const std::string& file_);

    void _writeTET(const std::string& file_);
};

}  // namespace phyanim

#endif
