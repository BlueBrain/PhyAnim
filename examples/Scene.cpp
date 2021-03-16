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
    , _renderMode(0), _anim(false), _meshVolume(0.0)
    , _collision(false), _framesCount(0), _simulationTime(0.0)
    , _meshUpdateTime(0.0) {

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
    switch(simSystem_) {
    case EXPLICITMASSSPRING:
        std::cout << "Explicit mass-spring simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ExplicitMassSpringSystem(_dt, _collDetect);
        break;
    case IMPLICITMASSSPRING:
        std::cout << "Implicit mass-spring simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ImplicitMassSpringSystem(_dt, _collDetect);
        break;
    case EXPLICITFEM:
        std::cout << "Explicit FEM simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ExplicitFEMSystem(_dt, _collDetect); 
        break;
    case IMPLICITFEM:
        std::cout << "Implicit FEM simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ImplicitFEMSystem(_dt, _collDetect); 
        break;
    }
    std::srand(std::time(0));
    _previousTime = std::chrono::steady_clock::now();
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
        ++_framesCount;

        _collDetect->dynamicMeshes(_meshes);
        auto startTime = std::chrono::steady_clock::now();
        bool collision = _animSys->step();
        _collision = _collision || collision;
        auto endTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = endTime-startTime; 
        _simulationTime += elapsedTime.count();

        startTime = std::chrono::steady_clock::now();
        for (size_t i = 0; i < _meshes.size(); i++) {
            auto mesh = dynamic_cast<phyanim::DrawableMesh*>(_meshes[i]);
            mesh->aabb->update();
            mesh->uploadNodes();
        }
        endTime = std::chrono::steady_clock::now();
        elapsedTime = endTime-startTime; 
        _meshUpdateTime+= elapsedTime.count();

        elapsedTime = endTime-_previousTime;
        if (elapsedTime.count() >= 1.0) {
            std::cout << "FPS: " << _framesCount << std::endl;
            if (_collision) {
                std::cout << "Collision detected" << std::endl;
            }
            std::cout << "Simulation average time per frame: " <<
                    _simulationTime / _framesCount <<
                    " seconds." << std::endl;
        
            std::cout << "Mesh update average time per frame: " <<
                    _meshUpdateTime / _framesCount << " seconds." << std::endl;
            for (auto mesh: _meshes) {
                std::cout << "Mesh original area: " << mesh->initArea <<
                        ". Area difference: " <<
                        (mesh->initArea/mesh->area()-1.0)*100.0 << "%" <<
                        std::endl;
                if (mesh->tetrahedra.size()>0) {
                    std::cout << "Mesh original volume: " << mesh->initVolume <<
                            ". Volume difference: " <<
                            (mesh->initVolume/mesh->volume()-1.0)*100.0 << "%" <<
                            std::endl;
                    // for (size_t i = 0; i < mesh->tetrahedra.size(); ++i)
                    // {
                    //     if (abs(mesh->tetrahedra[i]->volume() - _tetrahedraVolume[i]) >= 0.1 )
                    //         std::cout << "Volume diference in tetrahedra " << i << std::endl;
                    // }
                }
            }
            _collision = false;
            _previousTime = endTime;
            _framesCount = 0;
            _simulationTime = 0.0;
            _meshUpdateTime = 0.0;
        }


    }

    
    glUseProgram(_program);
    Eigen::Matrix4f projView = _camera->projectionViewMatrix().cast<float>();
    Eigen::Matrix4f view = _camera->viewMatrix().cast<float>();
    glUniformMatrix4fv(_uProjViewModel, 1, GL_FALSE, projView.data());
    glUniformMatrix4fv(_uViewModel, 1, GL_FALSE, view.data());
    for (auto mesh: _meshes) {
        dynamic_cast<phyanim::DrawableMesh*>(mesh)->renderSurface();
    }
    // _mesh->renderSurface();
}

void Scene::loadMesh(const std::string& file_) {
    auto mesh = new phyanim::DrawableMesh(_meshStiffness, _meshDensity,
                                          _meshDamping, _meshPoissonRatio);
    
    mesh->load(file_);
    _loadMesh(mesh);
}

void Scene::loadMesh(const std::string& nodeFile_, const std::string& eleFile_)
{
    auto mesh = new phyanim::DrawableMesh(_meshStiffness, _meshDensity,
                                          _meshDamping, _meshPoissonRatio);
    
    mesh->load(nodeFile_, eleFile_);
    _loadMesh(mesh);
}

void Scene::clear() {
    _animSys->clear();
    for (auto mesh: _meshes) {
        delete mesh;
    }
    _meshes.clear();
}

void Scene::restart() {
    for (auto baseMesh: _meshes) {
        auto mesh = dynamic_cast<phyanim::DrawableMesh*>(baseMesh);
        mesh->nodesToInitPos();
        mesh->uploadNodes();
    }
}

void Scene::gravity() {
    _animSys->gravity = !_animSys->gravity;
}

void Scene::collisions() {
    _animSys->collisions = !_animSys->collisions;
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

void Scene::_loadMesh(phyanim::DrawableMesh* mesh_)
{
    _animSys->addMesh(mesh_);
    mesh_->upload();
    _meshes.push_back(mesh_);
    std::cout << "Mesh loaded with: " << mesh_->nodes.size() <<
            " nodes, " << mesh_->edges.size() << " edges, " <<
            mesh_->tetrahedra.size() << " tetrahedra and "<<
            mesh_->surfaceTriangles.size() << " triangles" <<
            std::endl;

    _tetrahedraVolume = new double[mesh_->tetrahedra.size()];

    for (size_t i = 0; i < mesh_->tetrahedra.size(); ++i)
    {

        auto tet = mesh_->tetrahedra[i];
        _tetrahedraVolume[i] = tet->volume();

        // double threshold = 0.0001;
        // if (_tetrahedraVolume[i] < threshold)
        // {
        //     std::cout << "Tet id: " << i << // " " << tet->node0->id << " " << tet->node1->id <<
        //             // " " <<  tet->node2->id << " " <<  tet->node3->id <<
        //             " volume: " <<  _tetrahedraVolume[i] << std::endl; 
        // }
    }

                
    phyanim::AABB limits = mesh_->aabb->root->aabb;
    phyanim::Vec3 center = limits.center();

    
    center.z() = limits.upperLimit.z() * 2;
    _camera->position(center);

    center = limits.center();
    phyanim::Vec3 axis0 = (limits.lowerLimit-center)*4.0;
    limits.lowerLimit = center+axis0;
    limits.upperLimit = center-axis0;
    _collDetect->aabb = limits;

}

}
