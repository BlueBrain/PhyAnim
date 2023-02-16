#ifndef __EXAMPLES_MESH_H
#define __EXAMPLES_MESH_H

#include <Phyanim.h>

namespace examples
{
class Mesh
{
public:
    Mesh();

    virtual ~Mesh();

    void render(uint32_t primitiveType = 0x0004);

    void uploadIndices(std::vector<uint32_t> indices);
    void uploadPositions(std::vector<double> positions);
    void uploadNormals(std::vector<double> normals);
    void uploadColors(std::vector<double> colors);

private:
    std::vector<uint32_t> _indices;
    std::vector<double> _positions;
    std::vector<double> _normals;
    std::vector<double> _colors;

    bool _initialized;
    bool _updateIndices;
    bool _updatePositions;
    bool _updateNormals;
    bool _updateColors;

    void _init();
    void _upload();
    void _uploadIndices();
    void _uploadPositions();
    void _uploadNormals();
    void _uploadColors();

    uint32_t _vao;
    uint32_t _indexVbo;
    uint32_t _positionVbo;
    uint32_t _normalVbo;
    uint32_t _colorVbo;
    uint32_t _indexSize;
};

void setGeometry(Mesh* mesh, phyanim::Nodes& nodes);

void setColor(Mesh* mesh,
              uint32_t numNodes,
              phyanim::Vec3 color = phyanim::Vec3(0, 0.8, 0.8));

void setColorByCollision(
    Mesh* mesh,
    phyanim::Nodes& nodes,
    phyanim::Vec3 noCollisionColor = phyanim::Vec3(0, 0.8, 0.8),
    phyanim::Vec3 collisionColor = phyanim::Vec3(0.8, 0.2, 0));

void setIndices(Mesh* mesh, phyanim::Primitives& primitives);

Mesh* generateMesh(phyanim::Nodes& nodes,
                   phyanim::Primitives& primitives,
                   phyanim::Vec3 color = phyanim::Vec3(0, 0.8, 0.8));

void vec3ToVec(phyanim::Vec3 vec3, std::vector<double>& vec, uint32_t index);

void computeEdgeGeometry(phyanim::Edge* edge,
                         std::vector<double>& positions,
                         std::vector<double>& normals,
                         std::vector<double>& colors,
                         phyanim::Vec3 color,
                         phyanim::Vec3 collColor,
                         phyanim::Vec3 fixColor);

void computeEdgeIndices(std::vector<uint32_t>& indices, uint32_t offset);

void updateGeometry(Mesh* mesh,
                    phyanim::Edges& edges,
                    phyanim::Vec3 color = phyanim::Vec3(0.2, 0.8, 0.2),
                    phyanim::Vec3 collColor = phyanim::Vec3(0.8, 0.2, 0.2),
                    phyanim::Vec3 fixColor = phyanim::Vec3(0.2, 0.2, 0.2));

Mesh* generateMesh(phyanim::Edges& edges,
                   phyanim::Vec3 color = phyanim::Vec3(0.2, 0.8, 0.2),
                   phyanim::Vec3 collColor = phyanim::Vec3(0.8, 0.2, 0.2),
                   phyanim::Vec3 fixColor = phyanim::Vec3(0.2, 0.2, 0.2));

}  // namespace examples

#endif
