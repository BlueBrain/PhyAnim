#include <iostream>

#include <GL/glew.h>

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

Scene::Scene(Camera* camera_, float dt_, float meshStiffness_)
    : _camera(camera_), _dt(dt_), _meshStiffness(meshStiffness_)
    , _program(0), _uProjViewModel(-1), _uViewModel(-1) {
    _init();
}

Scene::~Scene() {
    if (_camera) {
        delete _camera;
    }
}

float Scene::dt() {
    return _dt;
}

void Scene::dt(float dt_) {
    _dt = dt_;
}       

void Scene::render() {
    _animSys->step(_dt);
    _mesh->loadNodes();
    glUseProgram(_program);
    glUniformMatrix4fv(_uProjViewModel, 1, GL_FALSE,
                       _camera->projectionViewMatrix().data());
    glUniformMatrix4fv(_uViewModel, 1, GL_FALSE,
                       _camera->viewMatrix().data());
    _mesh->renderSurface();
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

void Scene::_init() {

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

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

    float h = 5.0f;
    float lt = 2.0f;
    float ht = lt * 0.8165;
    float hte = lt * 0.866025404;
    phyanim::Node* n0 = new phyanim::Node(phyanim::Vec3(.0f, h+ht, .0f), 0, true);
    phyanim::Node* n1 = new phyanim::Node(phyanim::Vec3(.0f, h, hte*0.66f),
                                          1, true);
    phyanim::Node* n2 = new phyanim::Node(phyanim::Vec3(.5f*lt, h, hte*-0.33f),
                                          2, true);
    phyanim::Node* n3 = new phyanim::Node(phyanim::Vec3(-.5f*lt, h, hte*-0.33f),
                                          3, true);

    phyanim::Tetrahedron* tet = new phyanim::Tetrahedron(n0, n1, n2, n3);
    
    _mesh = new phyanim::DrawableMesh();

    _mesh->tetrahedra().push_back(tet);

    _mesh->tetsToNodes();
    _mesh->tetsToEdges();
    _mesh->tetsToTriangles();
    _mesh->load();
    _mesh->stiffness(_meshStiffness);

    _animSys = new phyanim::MassSpringSystem();
    
    _animSys->addMesh(_mesh);
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
