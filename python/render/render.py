from array import array
import string
from OpenGL.GL import *
from OpenGL.GL.shaders import *
from OpenGL_accelerate import *
import sys

from render.math import *


class AABoundingBox:

    def __init__(self):
        self.min = Vec3(sys.float_info.max)
        self.max = Vec3(sys.float_info.min)

    def center(self):
        return (self.min + self.max) * 0.5

    def radius(self):
        return (self.max - self.center()).norm()

    def add_pos(self, position: Vec3):
        self.min = self.min.min(position)
        self.max = self.max.max(position)


class Mesh:

    def __init__(self, lines, triangles, positions, normals, colors):

        self.vbo_position = -1
        self.vbo_normal = -1
        self.vbo_color = -1
        self.vbo_lines = -1
        self.vbo_triangles = -1
        self.vao_lines = -1
        self.vao_triangles = -1
        self.aabb = AABoundingBox()

        # Vertices
        self.update_positions(positions)
        self.update_normals(normals)
        self.update_colors(colors)

        # Lines
        self.num_lines = len(lines)*2
        if self.num_lines > 0:
            lines_vec = []
            for l in lines:
                lines_vec += [l.id0, l.id1]
            self.vao_lines = glGenVertexArrays(1)
            glBindVertexArray(self.vao_lines)

            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_position)
            glEnableVertexAttribArray(0)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                  12, ctypes.c_void_p(0))
            if self.vbo_normal >= 0:
                glBindBuffer(GL_ARRAY_BUFFER, self.vbo_normal)
                glEnableVertexAttribArray(1)
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                      12, ctypes.c_void_p(0))
            if self.vbo_color >= 0:
                glBindBuffer(GL_ARRAY_BUFFER, self.vbo_color)
                glEnableVertexAttribArray(2)
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                                      12, ctypes.c_void_p(0))

            lines_vec = np.array(lines_vec, dtype=np.uint32)
            self.vbo_lines = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_lines)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         lines_vec.nbytes, lines_vec, GL_STATIC_DRAW)
            glBindVertexArray(0)

        # Triangles
        self.num_triangles = len(triangles)*3
        if self.num_triangles > 0:
            triangles_vec = []
            for t in triangles:
                triangles_vec += [t.id0, t.id1, t.id2]
            self.vao_triangles = glGenVertexArrays(1)
            glBindVertexArray(self.vao_triangles)

            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_position)
            glEnableVertexAttribArray(0)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                  12, ctypes.c_void_p(0))
            if self.vbo_normal >= 0:
                glBindBuffer(GL_ARRAY_BUFFER, self.vbo_normal)
                glEnableVertexAttribArray(1)
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                      12, ctypes.c_void_p(0))
            if self.vbo_color >= 0:
                glBindBuffer(GL_ARRAY_BUFFER, self.vbo_color)
                glEnableVertexAttribArray(2)
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                                      12, ctypes.c_void_p(0))

            triangles_vec = np.array(triangles_vec, dtype=np.uint32)
            self.vbo_triangles = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_triangles)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         triangles_vec.nbytes, triangles_vec, GL_STATIC_DRAW)
            glBindVertexArray(0)

    def __del__(self):
        if (self.vbo_position >= 0):
            glDeleteBuffers(1, self.vbo_position)
        if (self.vbo_normal >= 0):
            glDeleteBuffers(1, self.vbo_normal)
        if (self.vbo_color >= 0):
            glDeleteBuffers(1, self.vbo_color)
        if (self.vbo_lines >= 0):
            glDeleteBuffers(1, self.vbo_lines)
        if (self.vbo_triangles >= 0):
            glDeleteBuffers(1, self.vbo_triangles)
        if (self.vao_lines >= 0):
            glDeleteVertexArrays(1, self.vao_lines)
        if (self.vao_triangles >= 0):
            glDeleteVertexArrays(1, self.vao_triangles)

    def update_positions(self, positions):
        if len(positions) > 0:
            positions_vec = []
            for position in positions:
                self.aabb.add_pos(position)
                positions_vec.append([position.x, position.y, position.z])
            if self.vbo_position < 0:
                self.vbo_position = glGenBuffers(1)
            positions_vec = np.array(positions_vec, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_position)
            glBufferData(GL_ARRAY_BUFFER, positions_vec.nbytes,
                         positions_vec, GL_STATIC_DRAW)

    def update_normals(self, normals):
        if len(normals) > 0:
            normals_vec = []
            for normal in normals:
                normals_vec.append([normal.x, normal.y, normal.z])
            if self.vbo_normal < 0:
                self.vbo_normal = glGenBuffers(1)
            normals_vec = np.array(normals_vec, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_normal)
            glBufferData(GL_ARRAY_BUFFER, normals_vec.nbytes,
                         normals_vec, GL_STATIC_DRAW)

    def update_colors(self, colors):
        if len(colors) > 0:
            colors_vec = []
            for color in colors:
                colors_vec.append([color.x, color.y, color.z])
            if self.vbo_color < 0:
                self.vbo_color = glGenBuffers(1)
            colors_vec = np.array(colors_vec, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_color)
            glBufferData(GL_ARRAY_BUFFER, colors_vec.nbytes,
                         colors_vec, GL_STATIC_DRAW)

    def render_lines(self):
        if self.vao_lines >= 0:
            glBindVertexArray(self.vao_lines)
            glDrawElements(GL_LINES, self.num_lines,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))

    def render_triangles(self):
        if self.vao_triangles >= 0:
            glBindVertexArray(self.vao_triangles)
            glDrawElements(GL_TRIANGLES, self.num_triangles,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))

    def render_patches(self):
        if self.vao_triangles >= 0:
            glPatchParameteri(GL_PATCH_VERTICES, 3)
            glBindVertexArray(self.vao_triangles)
            glDrawElements(GL_PATCHES, self.num_triangles,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))


class QuadMesh(Mesh):
    def __init__(self):
        lines = []
        triangles = [Triangle(0, 1, 2), Triangle(0, 2, 3)]
        positions = [Vec3(-1, 1, 0.0), Vec3(-1, -1, 0.0),
                     Vec3(1, -1, 0), Vec3(1, 1, 0.0)]
        normals = [Vec3(0, 0, 1), Vec3(0, 0, 1),
                   Vec3(0, 0, 1), Vec3(0, 0, 1)]
        colors = [Vec3(1, 1, 1), Vec3(1, 0.8, 1),
                  Vec3(1, 0.8, 1), Vec3(1, 1, 1)]
        Mesh.__init__(self, lines, triangles, positions, normals, colors)


class TriangleMesh(Mesh):
    def __init__(self):
        lines = []
        triangles = [Triangle(0, 1, 2)]
        positions = [Vec3(-1, 1, 0.0), Vec3(-1, -1, 0.0),
                     Vec3(1, -1, 0)]
        normals = [Vec3(0, 0, 1), Vec3(0, 0, 1),
                   Vec3(0, 0, 1)]
        colors = [Vec3(1, 0, 0), Vec3(0, 1, 0),
                  Vec3(0, 0, 1)]
        Mesh.__init__(self, lines, triangles, positions, normals, colors)


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
        self.primitives = primitives
        self.__create_program(shader_descriptions)

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
        if not self.id is None:
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
        self.background_color = Vec3(1.0)
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)
        self.color = Vec3(0.2, 0.8, 1.0)
        self._level = 1
        self._distance = 1
        self.models = []
        self.aabb = AABoundingBox()
        self.mode = GL_FILL
        glEnable(GL_CULL_FACE)
        glEnable(GL_DEPTH_TEST)

    @ property
    def background_color(self):
        return self._background_color

    @ background_color.setter
    def background_color(self, value):
        self._background_color = value
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)

    def add_model(self, model: Model):
        self.models.append(model)
        translation = Vec3(model[2].data[0, 3],
                           model[2].data[1, 3], model[2].data[2, 3])
        self.aabb.add_pos(model[0].aabb.min + translation)
        self.aabb.add_pos(model[0].aabb.max + translation)
        self.target = self.aabb.center()
        self.radius = self.aabb.radius()

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

    def render(self):
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
