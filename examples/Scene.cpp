#include <iostream>

#include <GL/glew.h>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/readPLY.h>
#include <igl/readOFF.h>

#include "Scene.h"

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
             double meshPoissonRatio_ )
    : _camera(camera_), _dt(dt_), _meshStiffness(meshStiffness_)
    , _meshDensity(meshDensity_), _meshDamping(meshDamping_)
    , _meshPoissonRatio(meshPoissonRatio_)
    , _program(0), _uProjViewModel(-1), _uViewModel(-1), _numRenderMode(2)
    , _renderMode(0), _anim(false) {

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

    switch(simSystem_) {
    case MASSSPRING:
        std::cout << "Mass-spring simulation with:\n\tdt " << dt_ <<
                "\n\tstiffness " << meshStiffness_ <<
                "\n\tdensity " << meshDensity_ << "\n\tdamping " <<
                meshDamping_ << std::endl;
                _animSys = new phyanim::MassSpringSystem();
        break;
    case FEM:
        std::cout << "FEM simulation with:\n\tdt " << dt_ <<
                "\n\tstiffness " << meshStiffness_ <<
                "\n\tdensity " << meshDensity_ << "\n\tdamping " <<
                meshDamping_ << "\n\tpoisson ratio " << meshPoissonRatio_ <<
                std::endl;
        _animSys = new phyanim::FEMSystem(); 
        break;
    }
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
        _mesh->loadNodes();
    }
    glUseProgram(_program);
    Eigen::Matrix4f projView = _camera->projectionViewMatrix().cast<float>();
    Eigen::Matrix4f view = _camera->viewMatrix().cast<float>();
    glUniformMatrix4fv(_uProjViewModel, 1, GL_FALSE, projView.data());
    glUniformMatrix4fv(_uViewModel, 1, GL_FALSE, view.data());
    _mesh->render();
    // _mesh->renderSurface();
}

void Scene::loadMesh(const std::string& file_) {
    if (file_.empty()) {
        std::cerr << "Empty mesh file error" << std::endl;
        return;
    }
    Eigen::MatrixXd sVertices;
    Eigen::MatrixXi sFacets;
        
    Eigen::MatrixXd vertices;
    Eigen::MatrixXi tets;
    Eigen::MatrixXi facets;

    if (file_.find(".off") != std::string::npos) {
        igl::readOFF(file_.c_str(), sVertices, sFacets);
    } else if (file_.find(".ply") != std::string::npos) {
        igl::readPLY(file_.c_str(), sVertices, sFacets);
    } else {
        return;
    }

    // std::cout << vertices << std::endl;
    // std::cout << facets << std::endl;        
    igl::copyleft::tetgen::tetrahedralize(sVertices,sFacets,"pq1.414Y",
                                          vertices, tets, facets);

    // vertices = sVertices;
    // facets = sFacets;
        
    size_t nVertices = vertices.rows();
    phyanim::Nodes rawNodes(nVertices);
    for (size_t i = 0; i < nVertices; ++i) {
        rawNodes[i] = new phyanim::Node(vertices.row(i), i);
    }

    size_t nTets = tets.rows();
    phyanim::Tetrahedra rawTets(nTets);
    for (size_t i = 0; i < nTets; ++i) {
        auto tet = new phyanim::Tetrahedron(
            rawNodes[tets(i,2)], rawNodes[tets(i,0)], rawNodes[tets(i,3)],
            rawNodes[tets(i,1)]);
        rawTets[i] = tet;
    }
        
    size_t nTriangles = facets.rows();
    phyanim::Triangles rawTriangles(nTriangles);
    for (size_t i = 0; i < nTriangles; ++i) {
        auto triangle = new phyanim::Triangle(
            rawNodes[facets(i,0)], rawNodes[facets(i,2)],
            rawNodes[facets(i,1)]); 
        rawTriangles[i] = triangle;
    }
    _mesh = new phyanim::DrawableMesh(_meshStiffness, _meshDensity,
                                      _meshDamping, _meshPoissonRatio);
    _mesh->nodes() = rawNodes;
    _mesh->tetrahedra() = rawTets;
    _mesh->surfaceTriangles() = rawTriangles;
    _mesh->tetsToEdges();
    _mesh->tetsToTriangles();
    
    std::cout << "Mesh loaded with: " << _mesh->nodes().size() <<
            " nodes, " << _mesh->edges().size() << " edges, " <<
            _mesh->tetrahedra().size() << " tetrahedra and "<<
            _mesh->surfaceTriangles().size() << " triangles" <<
            std::endl;

    _mesh->load();
    _animSys->addMesh(_mesh);
}

void Scene::restart() {
    _mesh->nodesToInitPos();
    _mesh->loadNodes();
}

void Scene::gravity() {
    _animSys->gravity(!_animSys->gravity());
}

void Scene::floorCollision() {
    _animSys->floorCollision(!_animSys->floorCollision());
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
