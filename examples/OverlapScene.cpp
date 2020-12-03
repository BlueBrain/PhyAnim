#include <iostream>
#include <cstdlib>   
#include <ctime>

#include <GL/glew.h>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/readPLY.h>
#include <igl/readOFF.h>
#include <igl/readOBJ.h>
#include <igl/writeOFF.h>

#include "OverlapScene.h"
#include <ExplicitMassSpringSystem.h>
#include <ImplicitMassSpringSystem.h>
#include <ExplicitFEMSystem.h>
#include <ImplicitFEMSystem.h>



namespace examples {

const std::string vshaderSource(
    "#version 400\n"
    "in vec3 inPos;"
    "out vec3 position;"
    "uniform mat4 projViewModel;"
    "uniform mat4 viewModel;"
    "void main(void) {"
    "position = (viewModel*vec4(inPos, 1.0)).xyz;"
    "gl_Position = projViewModel * vec4(inPos, 1.0);}");

const std::string gshaderSource(
    "#version 400\n"
    "layout(triangles) in;"
    "layout(triangle_strip, max_vertices = 3) out;"
    "in vec3 position[3];"
    "out vec3 normal;"
    "out vec3 gposition;"
    "void main(void) {"
    "vec3 axis0 = normalize(position[1]-position[0]);"
    "vec3 axis1 = normalize(position[2]-position[0]);"
    "normal = cross(axis0, axis1);"
    "gposition = position[0];"
    "gl_Position = gl_in[0].gl_Position;"
    "EmitVertex();"
    "gposition = position[1];"
    "gl_Position = gl_in[1].gl_Position;"
    "EmitVertex();"
    "gposition = position[2];"
    "gl_Position = gl_in[2].gl_Position;"
    "EmitVertex();"
    "EndPrimitive();}");

const std::string fshaderSource(
    "#version 400\n"
    "in vec3 normal;"
    "in vec3 gposition;"
    "out vec4 oColor;"
    "void main(void){"
    "vec3 N = normalize( normal );"
    "vec3 L = normalize( -gposition );"
    "float diff = dot( N, L );"
    "diff = clamp( diff, 0.0, 1.0 );"
    "vec3 color = vec3( 0.5, 0.8, 0.2 );"
    "oColor = vec4( diff*color*0.8+color*0.2, 1.0 );}");

OverlapScene::OverlapScene(Camera* camera_, SimSystem simSystem_, double dt_,
             double meshStiffness_,  double meshDensity_, double meshDamping_,
             double meshPoissonRatio_, double collisionStiffness_ )
    : _camera(camera_), _dt(dt_), _meshStiffness(meshStiffness_)
    , _meshDensity(meshDensity_), _meshDamping(meshDamping_)
    , _meshPoissonRatio(meshPoissonRatio_)
    , _collisionStiffness(collisionStiffness_)
    , _program(0), _uProjViewModel(-1), _uViewModel(-1), _numRenderMode(2)
    , _renderMode(0), _anim(true) {

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    _program = glCreateProgram();
    auto vshader = _compileShader(vshaderSource, GL_VERTEX_SHADER);
    auto gshader = _compileShader(gshaderSource, GL_GEOMETRY_SHADER);
    auto fshader = _compileShader(fshaderSource, GL_FRAGMENT_SHADER);

    if (vshader==0 || fshader==0) {
        std::cerr << "Bad shaders compilation" << std::endl;
    }
    glAttachShader(_program, vshader);
    glAttachShader(_program, gshader);
    glAttachShader(_program, fshader);
    glLinkProgram(_program);

    _uProjViewModel = glGetUniformLocation(_program, "projViewModel");
    _uViewModel = glGetUniformLocation(_program, "viewModel");

    _collDetect = new phyanim::CollisionDetection(_collisionStiffness);
    _collDetect->aabb = phyanim::AABB(phyanim::Vec3(-100.0, -100.0, -100.0),
                                      phyanim::Vec3(100.0, 100.0, 100.0));
    
    switch(simSystem_) {
    case EXPLICITMASSSPRING:
        std::cout << "Explicit mass-spring simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ExplicitMassSpringSystem(_collDetect);
        break;
    case IMPLICITMASSSPRING:
        std::cout << "Implicit mass-spring simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ImplicitMassSpringSystem(_collDetect);
        break;
    case EXPLICITFEM:
        std::cout << "Explicit FEM simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ExplicitFEMSystem(_collDetect); 
        break;
    case IMPLICITFEM:
        std::cout << "Implicit FEM simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ImplicitFEMSystem(_collDetect);
        break;
    }
    
    _animSys->gravity = false;
    _startTime = std::chrono::steady_clock::now();
}

OverlapScene::~OverlapScene() {
    if (_camera) {
        delete _camera;
    }
}

double OverlapScene::dt() {
    return _dt;
}

void OverlapScene::dt(float dt_) {
    _dt = dt_;
}       

void OverlapScene::render() {
    if (_anim) {
        bool collision = _animSys->step(_dt);
        for (size_t i = 0; i < _meshes.size(); i++) {
            auto mesh = _meshes[i];
            mesh->loadNodes();
        }

        if (!collision)
        {
            _anim = false;
            auto endTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsedTime = endTime-_startTime;
            std::cout << "Overlap solved in: " << elapsedTime.count() << "seconds" << std::endl;

            for (unsigned int i = 0; i < _meshes.size(); i++) {
                auto mesh = _meshes[i];
                std::cout << "Mesh original volume: " << mesh->initVolume <<
                        ". Volume difference: " <<
                        (mesh->initVolume/mesh->volume()-1.0)*100.0 << "%" <<
                        std::endl;
                std::string file("mesh" + std::to_string(i) + "solution.off");
                writeMesh(file, mesh);
            }
        }
    }

    glUseProgram(_program);
    Eigen::Matrix4f projView = _camera->projectionViewMatrix().cast<float>();
    Eigen::Matrix4f view = _camera->viewMatrix().cast<float>();
    glUniformMatrix4fv(_uProjViewModel, 1, GL_FALSE, projView.data());
    glUniformMatrix4fv(_uViewModel, 1, GL_FALSE, view.data());
    for (auto mesh: _meshes) {
        mesh->renderSurface();
    }
    // _mesh->renderSurface();
}

void OverlapScene::loadMesh(const std::string& file_, phyanim::Vec3 translation_ ) {
    if (file_.empty()) {
        std::cerr << "Empty mesh file error" << std::endl;
        return;
    }
    Eigen::MatrixXd vertices;
    Eigen::MatrixXi facets;
    
    if (file_.find(".off") != std::string::npos) {
        igl::readOFF(file_.c_str(), vertices, facets);
    } else if (file_.find(".ply") != std::string::npos) {
        igl::readPLY(file_.c_str(), vertices, facets);
    } else if (file_.find(".obj") != std::string::npos) {
        igl::readOBJ(file_.c_str(), vertices, facets);
    } else {
        return;
    }
        
    size_t nVertices = vertices.rows();
    phyanim::Nodes rawNodes(nVertices);

    for (size_t i = 0; i < nVertices; ++i) {
        phyanim::Vec3 pos(vertices.row(i));
        rawNodes[i] = new phyanim::Node( pos+translation_, i);
    }

    size_t nTets = facets.rows();
    phyanim::Tetrahedra rawTets(nTets);
    for (size_t i = 0; i < nTets; ++i) {
        auto tet = new phyanim::Tetrahedron(
            rawNodes[facets(i,0)], rawNodes[facets(i,1)], rawNodes[facets(i,3)],
            rawNodes[facets(i,2)]);
        rawTets[i] = tet;
    }
        
    // size_t nTriangles = facets.rows();
    // phyanim::Triangles rawTriangles(nTriangles);
    // for (size_t i = 0; i < nTriangles; ++i) {
    //     auto triangle = new phyanim::Triangle(
    //         rawNodes[facets(i,0)], rawNodes[facets(i,2)],
    //         rawNodes[facets(i,1)]); 
    //     rawTriangles[i] = triangle;
    // }
    auto mesh = new phyanim::DrawableMesh(_meshStiffness, _meshDensity,
                                      _meshDamping, _meshPoissonRatio);
    mesh->nodes = rawNodes;
    mesh->tetrahedra = rawTets;
    // mesh->surfaceTriangles() = rawTriangles;
    mesh->tetsToEdges();
    mesh->tetsToTriangles();
    
    std::cout << "Mesh loaded with: " << mesh->nodes.size() <<
            " nodes, " << mesh->edges.size() << " edges, " <<
            mesh->tetrahedra.size() << " tetrahedra and "<<
            mesh->surfaceTriangles.size() << " triangles" <<
            std::endl;

    _animSys->addMesh(mesh);
    mesh->load();
    mesh->initVolume = mesh->volume();
    _meshes.push_back(mesh);
    
    std::string outName(file_ + "inFile.off");
    writeMesh(outName, mesh);
}

void OverlapScene::writeMesh(const std::string& file_, phyanim::Mesh* mesh_) {

    auto nodes = mesh_->nodes;
    auto triangles = mesh_->surfaceTriangles;
    unsigned int nVertices = nodes.size();
    unsigned int nFacets = triangles.size();
    Eigen::MatrixXd vertices(nVertices,3);
    Eigen::MatrixXi facets(nFacets,3);

    for (unsigned int i=0; i < nVertices; i++) {
        nodes[i]->id = i;
        vertices.row(i) = nodes[i]->position;
    }
    for (unsigned int i=0; i < nFacets; i++) {
        Eigen::MatrixXi facet(1,3);
        facet << triangles[i]->node0->id, triangles[i]->node1->id,
                triangles[i]->node2->id; 
        facets.row(i) = facet;
    }

    igl::writeOFF(file_.c_str(), vertices, facets);
}

void OverlapScene::clear() {
    _animSys->clear();
    for (auto mesh: _meshes) {
        delete mesh;
    }
    _meshes.clear();
}

void OverlapScene::restart() {
    for (auto mesh: _meshes) {
        mesh->nodesToInitPos();
        mesh->loadNodes();
    }
}

void OverlapScene::gravity() {
    _animSys->gravity = !_animSys->gravity;
}

void OverlapScene::collisions() {
    _animSys->collisions = !_animSys->collisions;
}

void OverlapScene::changeRenderMode() {
    ++_renderMode;
    _renderMode %= _numRenderMode;

    switch (_renderMode) {
    case 0:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 1:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    }
}

bool OverlapScene::anim() {
    return _anim;
}

void OverlapScene::anim(bool anim_) {
    _anim = anim_;
}
        

unsigned int OverlapScene::_compileShader(const std::string& source_,
                                   int type_) {
    unsigned int shader;
    shader = glCreateShader(type_);
    const char* cSource = source_.c_str();
    glShaderSource(shader, 1, &cSource, nullptr);

    int status;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
      int length;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
      char* infoLog = new char[length];
      glGetShaderInfoLog(shader, length, nullptr, infoLog);
      std::cerr << "Shader compile error: " << infoLog << std::endl;
      delete[] infoLog;
      return 0;
    }
    return shader;
  }

}
