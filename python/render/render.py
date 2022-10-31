import string
import threading
import time
from OpenGL.GL.shaders import *
from render.mesh import *


class ShaderType:
    VERTEX = GL_VERTEX_SHADER
    FRAGMENT = GL_FRAGMENT_SHADER
    GEOMETRY = GL_GEOMETRY_SHADER
    TESS_CONTROL = GL_TESS_CONTROL_SHADER
    TESS_EVALUATION = GL_TESS_EVALUATION_SHADER


ShaderDescription = (string, ShaderType)


class ShaderProgram:

    def __init__(self, shader_descriptions, primitives):
        self.id = None
        self.shaders = []
        self.shader_descriptions = shader_descriptions
        self.primitives = primitives
        self._initialized = False

    def __del__(self):
        if not self.id is None:
            glDeleteProgram(self.id)
        for shader in self.shaders:
            glDeleteShader(shader)

    def __create_shader(self, shader_desc: ShaderDescription):
        with open(shader_desc[0], 'r') as f:
            shader_source = f.readlines()
            return compileShader(shader_source, shader_desc[1])
        return None

    def __create_program(self, shader_descriptions):
        self.shaders = []
        self.id = glCreateProgram()
        for shader_desc in shader_descriptions:
            shader = self.__create_shader(shader_desc)
            if not shader is None:
                self.shaders.append(shader)
                glAttachShader(self.id, shader, shader_desc[1])
        glLinkProgram(self.id)
        self.__register_uniforms()

    def __register_uniforms(self):
        self.uniforms = {}
        count = glGetProgramiv(self.id, GL_ACTIVE_UNIFORMS)
        for i in range(count):
            info = glGetActiveUniform(self.id, i)
            name = info[0].decode('UTF-8')
            location = glGetUniformLocation(self.id, name)
            self.uniforms[name] = location

    def use(self):
        if not self._initialized:
            self.__create_program(self.shader_descriptions)
            self._initialized = True
        if self.id:
            glUseProgram(self.id)


class Camera:

    def __init__(self, target: Vec3 = Vec3(0),
                 rotation: Mat3 = Mat3([1, 0, 0, 0, 1, 0, 0, 0, 1]),
                 radius: float = 1.0,  up: Vec3 = Vec3(0, 1, 0), near: float = 0.01,
                 far: float = 10000.0, fov: float = 90.0, ratio: float = 1.0):
        self._target = target
        self._rotation = rotation
        self._radius = radius
        self._up = up
        self._near = near
        self._far = far
        self._fov = fov * math.pi / 360.0
        self._ratio = ratio
        self.__build_view()
        self.__build_proj()

    def __build_view(self):
        dir = Vec3(0, 0, 1)
        r = self._rotation
        r_t = r.transpose()
        dir = r_t * dir
        pos = r * (self._target + dir * self._radius)
        self.view = Mat4([r.data[0][0], r.data[0][1],
                          r.data[0][2], -pos.x,
                          r.data[1][0], r.data[1][1],
                          r.data[1][2], -pos.y,
                          r.data[2][0], r.data[2][1],
                          r.data[2][2], -pos.z,
                          0, 0, 0, 1])

    def __build_proj(self):
        nf = 1.0 / (self._near - self._far)
        f = 1.0 / math.tan(self._fov)
        data = [f / self.ratio, 0.0, 0.0, 0.0,
                0.0, f, 0.0, 0.0,
                0.0, 0.0, (self._far + self._near) * nf,  2.0 *
                self._far * self._near * nf,
                0.0, 0.0, -1.0, 0.0]
        self.proj = Mat4(data)

    @property
    def target(self):
        return self._target

    @target.setter
    def target(self, target: Vec3):
        self._target = target
        self.__build_view()

    @property
    def rotation(self):
        return self._rotation

    @rotation.setter
    def rotation(self, value: Mat3):
        self._rotation = value
        self.__build_view()

    @property
    def radius(self):
        return self._radius

    @radius.setter
    def radius(self, value: float):
        self._radius = value
        self.__build_view()

    @property
    def ratio(self):
        return self._ratio

    @ratio.setter
    def ratio(self, value: float):
        self._ratio = value
        self.__build_proj()


Model = (Mesh, ShaderProgram, Mat4)


class Scene:
    def __init__(self):
        self.programs = []
        self.camera = Camera()
        self.background_color = Vec3(0.2)
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)
        self.color = Vec3(0.2, 0.8, 1.0)
        self._level = 1
        self._distance = 1
        self.models = []
        self.aabb = AABoundingBox()
        self.mode = GL_FILL
        self._update_models = False
        glEnable(GL_CULL_FACE)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_MULTISAMPLE)

    @ property
    def background_color(self):
        return self._background_color

    @ background_color.setter
    def background_color(self, value):
        self._background_color = value
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)

    def update_models(self):
        if self._update_models:
            for model in self.models:
                program = model[1]
                if not program in self.programs:                    
                    self.programs.append(program)
                    program.use()
                    uniform = program.uniforms.get("color")
                    if not uniform is None:
                        glUniform3fv(uniform, 1, self.color.data)
                    uniform = program.uniforms.get("view")
                    if not uniform is None:
                        glUniformMatrix4fv(uniform, 1, 1, self.camera.view.data)
                    uniform = program.uniforms.get("proj")
                    if not uniform is None:
                        glUniformMatrix4fv(uniform, 1, 1, self.camera.proj.data)
                    uniform = program.uniforms.get("maxLevel")
                    if not uniform is None:
                        glUniform1f(uniform, self._level)
                    uniform = program.uniforms.get("maxDistance")
                    if not uniform is None:
                        glUniform1f(uniform, self._distance)
            self._update_models = False
            self.target = self.aabb.center()
            self.radius = self.aabb.radius()


    def add_model(self, model: Model):
        self.models.append(model)
        self._update_models = True
        translation = Vec3(model[2].data[0, 3],
                           model[2].data[1, 3], model[2].data[2, 3])
        
        self.aabb.add_aabb(model[0].aabb, translation)

    def render(self):
        self.update_models()
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        for model in self.models:
            mesh = model[0]
            program = model[1]
            mat = model[2]

            program.use()
            uniform = program.uniforms.get("model")
            if not uniform is None:
                glDepthMask(GL_TRUE)
                glUniformMatrix4fv(uniform, 1, 1, mat.data)
            else:
                glDepthMask(GL_FALSE)
            if (program.primitives == GL_LINES):
                mesh.render_lines()
            elif (program.primitives == GL_TRIANGLES):
                mesh.render_triangles()
            elif (program.primitives == GL_PATCHES):
                mesh.render_patches()


    def render_mode(self):
        if self.mode == GL_FILL:
            self.mode = GL_LINE
        else:
            self.mode = GL_FILL
        glPolygonMode(GL_FRONT_AND_BACK, self.mode)

    @property
    def target(self):
        return self.camera.target

    @target.setter
    def target(self, target: Vec3):
        self.camera.target = target
        for program in self.programs:
            program.use()
            uniform = program.uniforms.get("view")
            if not uniform is None:
                glUniformMatrix4fv(uniform, 1, 1, self.camera.view.data)

    @property
    def rotation(self):
        return self.camera.rotation

    @rotation.setter
    def rotation(self, rotation: Mat3):
        self.camera.rotation = rotation
        for program in self.programs:
            program.use()
            uniform = program.uniforms.get("view")
            if not uniform is None:
                glUniformMatrix4fv(uniform, 1, 1, self.camera.view.data)

    @property
    def radius(self):
        return self.camera.radius

    @radius.setter
    def radius(self, value: float):
        self.camera.radius = value
        for program in self.programs:
            program.use()
            uniform = program.uniforms.get("view")
            if not uniform is None:
                glUniformMatrix4fv(uniform, 1, 1, self.camera.view.data)
    
    @property
    def ratio(self):
        return self.camera.ratio

    @ratio.setter
    def ratio(self, value: float):
        self.camera.ratio = value
        for program in self.programs:
            program.use()
            uniform = program.uniforms.get("proj")
            if not uniform is None:
                glUniformMatrix4fv(uniform, 1, 1, self.camera.proj.data)
    
    @property
    def level(self):
        return self._level

    @level.setter
    def level(self, level):
        self._level = level
        if self._level < 1:
            self._level = 1
        for program in self.programs:
            program.use()
            uniform = program.uniforms.get("maxLevel")
            if not uniform is None:
                glUniform1f(uniform, self._level)
    
    @property
    def distance(self):
        return self._distance

    @distance.setter
    def distance(self, distance):
        self._distance = distance
        if self._distance <= 0:
            self._distance = 0.001
        for program in self.programs:
            program.use()
            uniform = program.uniforms.get("maxDistance")
            if not uniform is None:
                glUniform1f(uniform, self._distance)
    