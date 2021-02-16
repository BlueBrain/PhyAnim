#include <iostream>
#include <cstdlib>   
#include <ctime>

#include <GL/glew.h>

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

OverlapScene::OverlapScene(const std::vector<std::string>& files_,
                           Camera* camera_, SimSystem simSystem_, double dt_,
                           double meshStiffness_,  double meshDensity_,
                           double meshDamping_, double meshPoissonRatio_,
                           double collisionStiffness_ )
    : _files(files_), _camera(camera_), _dt(dt_), _meshStiffness(meshStiffness_)
    , _meshDensity(meshDensity_), _meshDamping(meshDamping_)
    , _meshPoissonRatio(meshPoissonRatio_)
    , _collisionStiffness(collisionStiffness_), _solved(false)
    , _program(0), _uProjViewModel(-1), _uViewModel(-1), _numRenderMode(2)
    , _renderMode(0), anim(true) {

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
    case IMPLICITFEM:
        std::cout << "Implicit FEM simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ImplicitFEMSystem(_collDetect);
        break;
        
    default:
        std::cout << "Explicit FEM simulation with:\n\tdt " << _dt <<
                "\n\tstiffness " << _meshStiffness <<
                "\n\tdensity " << _meshDensity << "\n\tdamping " <<
                _meshDamping << "\n\tpoisson ratio " << _meshPoissonRatio <<
                "\n\tcollision stiffness " << _collisionStiffness <<
                std::endl;
        _animSys = new phyanim::ExplicitFEMSystem(_collDetect); 
        break;
    }
    _animSys->gravity = false;
    _startTime = std::chrono::steady_clock::now();
    _mesh = nullptr;
  
}

OverlapScene::~OverlapScene() {
    if (_camera) {
        delete _camera;
    }
}


void OverlapScene::render() {
    if (anim)
    {
        if (!_solved){
            if(_mesh) {
                bool collision = _animSys->step(_dt);
                _mesh->aabb->update();
                phyanim::DrawableMesh* drawableMesh =
                        dynamic_cast<phyanim::DrawableMesh*>(_mesh);
                drawableMesh->uploadNodes();
                
                if (!collision)
                {
                    double mean, max, min, rms;
                    _mesh->positionDifference(mean, max, min, rms);
                    std::cout << "Mesh " + _file + ":" << std::endl;
                    std::cout << "\t* Original volume: " <<
                            _mesh->initVolume << ". Volume difference: " <<
                            (_mesh->initVolume/_mesh->volume()-1.0)*100.0 << "%" <<
                            std::endl;
                    std::cout << "\t* Vertices distance mean: " << mean <<
                            " max: " << max << " min: " << min << " rms: " << rms <<
                            std::endl;
                    std::string file( _file + "solution.off");
                    _mesh->write(file);
                    _meshes.push_back(_mesh);
                    _mesh = nullptr;
                }
            } else {
                if (!_files.empty()){
                    _file = _files[0];
                    _files.erase(_files.begin());
                    if (_file.find(".node") != std::string::npos) {
                        _file1 = _files[0];
                        _files.erase(_files.begin());
                        _mesh = _loadMesh(_file, _file1);
                    } else {
                        _mesh = _loadMesh(_file);
                    }
                    phyanim::Meshes dMesh;
                    dMesh.push_back(_mesh);
                
                    _animSys->clear();
                    _animSys->addMesh(_mesh);
                    _collDetect->dynamicMeshes(dMesh);
                    _collDetect->staticMeshes(_meshes);
                } else {
                    auto endTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsedTime = endTime-_startTime;
                    std::cout << "Overlap solved in: " << elapsedTime.count() <<
                            "seconds" << std::endl;
                    _solved = true;
                }
            }
        }
    }
    glUseProgram(_program);
    Eigen::Matrix4f projView = _camera->projectionViewMatrix().cast<float>();
    Eigen::Matrix4f view = _camera->viewMatrix().cast<float>();
    glUniformMatrix4fv(_uProjViewModel, 1, GL_FALSE, projView.data());
    glUniformMatrix4fv(_uViewModel, 1, GL_FALSE, view.data());
    for(auto mesh: _meshes) {
        auto drawableMesh = dynamic_cast<phyanim::DrawableMesh*>(mesh);
        drawableMesh->renderSurface();
    }

    if (_mesh) {
        auto drawableMesh = dynamic_cast<phyanim::DrawableMesh*>(_mesh);
        drawableMesh->renderSurface();
    }
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


phyanim::DrawableMesh* OverlapScene::_loadMesh(const std::string& file_) {
    phyanim::DrawableMesh* mesh = new phyanim::DrawableMesh(
        _meshStiffness, _meshDensity, _meshDamping, _meshPoissonRatio);
    mesh->load(file_);
    mesh->upload();

    phyanim::AABB limits = mesh->aabb->root->aabb;

    phyanim::Vec3 center = limits.center(); 
    phyanim::Vec3 axis0 = (limits.lowerLimit-center);
    limits.lowerLimit = center+axis0;
    limits.upperLimit = center-axis0;
    _collDetect->aabb = limits;
    center.z() = limits.upperLimit.z()+limits.upperLimit.x()-center.x();
    _camera->position(center);    
    
    return mesh;
}

phyanim::DrawableMesh* OverlapScene::_loadMesh(const std::string& nodeFile_,
                                               const std::string& eleFile_) {
    phyanim::DrawableMesh* mesh = new phyanim::DrawableMesh(
        _meshStiffness, _meshDensity, _meshDamping, _meshPoissonRatio);
    mesh->load(nodeFile_, eleFile_);
    mesh->upload();

    phyanim::AABB limits = mesh->aabb->root->aabb;
    phyanim::Vec3 center = limits.center();

    center.z() = limits.upperLimit.z();
    _camera->position(center);    

    center = limits.center();
    phyanim::Vec3 axis0 = (limits.lowerLimit-center)*2.0;
    limits.lowerLimit = center+axis0;
    limits.upperLimit = center-axis0;
    _collDetect->aabb = limits;
    
    return mesh;
}

}
