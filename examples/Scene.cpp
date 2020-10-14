#include <iostream>
#include <cstdlib>   
#include <ctime>

#include <GL/glew.h>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/readPLY.h>
#include <igl/readOFF.h>

#include "Scene.h"
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

Scene::Scene(Camera* camera_, SimSystem simSystem_, double dt_,
             double meshStiffness_,  double meshDensity_, double meshDamping_,
             double meshPoissonRatio_, double collisionStiffness_ )
    : _camera(camera_), _dt(dt_), _meshStiffness(meshStiffness_)
    , _meshDensity(meshDensity_), _meshDamping(meshDamping_)
    , _meshPoissonRatio(meshPoissonRatio_)
    , _collisionStiffness(collisionStiffness_)
    , _program(0), _uProjViewModel(-1), _uViewModel(-1), _numRenderMode(2)
    , _renderMode(0), _anim(false), _meshVolume(0.0) {

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
    _collDetect->lowerLimit = phyanim::Vec3(-5.0, -5.0, -5.0);
    _collDetect->upperLimit = phyanim::Vec3(5.0, 10.0, 5.0);
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
    std::srand(std::time(0));
}

Scene::~Scene() {
    if (_camera) {
        delete _camera;
    }
}

double Scene::dt() {
    return _dt;
}

void Scene::dt(float dt_) {
    _dt = dt_;
}       

void Scene::render() {
    if (_anim) {
        _animSys->step(_dt);
        for (size_t i = 0; i < _meshes.size(); i++) {
            auto mesh = _meshes[i];
            mesh->loadNodes();
            std::cout << "Mesh original volume: " << mesh->initVolume <<
                    " volume difference: " <<
                    mesh->volume() - mesh->initVolume << std::endl;
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

void Scene::loadMesh(const std::string& file_) {
    if (file_.empty()) {
        std::cerr << "Empty mesh file error" << std::endl;
        return;
    }
    Eigen::MatrixXd vertices;
    Eigen::MatrixXi facets;
        
    // Eigen::MatrixXd vertices;
    // Eigen::MatrixXi tets;
    // Eigen::MatrixXi facets;

    if (file_.find(".off") != std::string::npos) {
        igl::readOFF(file_.c_str(), vertices, facets);
    // } else if (file_.find(".ply") != std::string::npos) {
    //     igl::readPLY(file_.c_str(), sVertices, sFacets);
    } else {
        return;
    }

    // std::cout << vertices << std::endl;
    // std::cout << facets << std::endl;        
    // igl::copyleft::tetgen::tetrahedralize(sVertices,sFacets,"pq1.414Y",
    //                                       vertices, tets, facets);

    // vertices = sVertices;
    // facets = sFacets;
        
    size_t nVertices = vertices.rows();
    phyanim::Nodes rawNodes(nVertices);

    double randNorm = 1.0 / RAND_MAX;

    phyanim::Vec3 randVelocity(std::rand()*randNorm*2.0-1.0,
                               std::rand()*randNorm*2.0-1.0,
                               std::rand()*randNorm*2.0-1.0);
    std::cout << randVelocity << std::endl;
    for (size_t i = 0; i < nVertices; ++i) {
        rawNodes[i] = new phyanim::Node(vertices.row(i), i, randVelocity);
    }

    size_t nTets = facets.rows();
    phyanim::Tetrahedra rawTets(nTets);
    for (size_t i = 0; i < nTets; ++i) {
        auto tet = new phyanim::Tetrahedron(
            rawNodes[facets(i,0)], rawNodes[facets(i,1)], rawNodes[facets(i,2)],
            rawNodes[facets(i,3)]);
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
}

void Scene::clear() {
    _animSys->clear();
    for (auto mesh: _meshes) {
        delete mesh;
    }
    _meshes.clear();
}

void Scene::restart() {
    for (auto mesh: _meshes) {
        mesh->nodesToInitPos();
        mesh->loadNodes();
    }
}

void Scene::gravity() {
    _animSys->gravity = !_animSys->gravity;
}

void Scene::floorCollision() {
    _animSys->limitsCollision = !_animSys->limitsCollision;
}

void Scene::changeRenderMode() {
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

bool Scene::anim() {
    return _anim;
}

void Scene::anim(bool anim_) {
    _anim = anim_;
}
        

unsigned int Scene::_compileShader(const std::string& source_,
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
