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
        positions_vec = []
        for position in positions:
            self.aabb.add_pos(position)
            positions_vec.append([position.x, position.y, position.z])
        self.vbo_position = glGenBuffers(1)
        positions_vec = np.array(positions_vec, dtype=np.float32)
        glBindBuffer(GL_ARRAY_BUFFER, self.vbo_position)
        glBufferData(GL_ARRAY_BUFFER, positions_vec.nbytes,
                     positions_vec, GL_STATIC_DRAW)
        if len(normals) > 0:
            normals_vec = []
            for normal in normals:
                normals_vec.append([normal.x, normal.y, normal.z])
            self.vbo_normal = glGenBuffers(1)
            normals_vec = np.array(normals_vec, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_normal)
            glBufferData(GL_ARRAY_BUFFER, normals_vec.nbytes,
                         normals_vec, GL_STATIC_DRAW)
        if len(colors) > 0:
            colors_vec = []
            for color in colors:
                colors_vec.append([color.x, color.y, color.z])
            self.vbo_color = glGenBuffers(1)
            colors_vec = np.array(colors_vec, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_color)
            glBufferData(GL_ARRAY_BUFFER, colors_vec.nbytes,
                         colors_vec, GL_STATIC_DRAW)

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

    def render(self):
        if self.vao_lines >= 0:
            glBindVertexArray(self.vao_lines)
            glDrawElements(GL_LINES, self.num_lines,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))
        if self.vao_triangles >= 0:
            glBindVertexArray(self.vao_triangles)
            glDrawElements(GL_TRIANGLES, self.num_triangles,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))


class Quad(Mesh):
    def __init__(self):
        lines = []
        triangles = [Triangle(0, 1, 2), Triangle(0, 2, 3)]
        positions = [Vec3(-1, 1, 0.0), Vec3(-1, -1, 0.0),
                     Vec3(1, -1, 0), Vec3(1, 1, 0.0)]
        normals = [Vec3(0, 0, 1), Vec3(0, 0, 1),
                   Vec3(0, 0, 1), Vec3(0, 0, 1)]
        colors = [Vec3(0.0, 1, 1), Vec3(0, 0.5, 1),
                  Vec3(0, 0.5, 1), Vec3(0.0, 1, 1)]
        Mesh.__init__(self, lines, triangles, positions, normals, colors)


class ShaderType:
    VERTEX = GL_VERTEX_SHADER
    FRAGMENT = GL_FRAGMENT_SHADER
    GEOMETRY = GL_GEOMETRY_SHADER


ShaderDescription = (string, ShaderType)


class ShaderProgram:

    def __init__(self, shader_descriptions):
        self.id = None
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

    def __init__(self, target: Vec3 = Vec3(0), direction: Vec3 = Vec3(0, 0, -1),
                 radius: float = 1.0,  up: Vec3 = Vec3(0, 1, 0), near: float = 0.01, far: float = 10000.0, fov: float = 90.0, ratio: float = 1.0):
        self._target = target
        self._direction = direction
        self._radius = radius
        self._up = up
        self._near = near
        self._far = far
        self._fov = fov * math.pi / 360.0
        self._ratio = ratio

        self.__build_view()
        self.__build_proj()

    def __build_view(self):
        dir = self._direction
        right = dir.cross(self._up).normalized()
        up = right.cross(dir).normalized()

        rot = Mat3([right.x, right.y, right.z,
                    up.x, up.y, up.z,
                    -dir.x, -dir.y, -dir.z])

        pos = rot * (self._target - self._direction * self._radius)

        self.view = Mat4([rot.data[0][0], rot.data[0][1],
                          rot.data[0][2], -pos.x,
                          rot.data[1][0], rot.data[1][1],
                          rot.data[1][2], -pos.y,
                          rot.data[2][0], rot.data[2][1],
                          rot.data[2][2], -pos.z,
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
    def direction(self):
        return self._direction

    @direction.setter
    def direction(self, value: Vec3):
        print(self._direction)
        self._direction = value
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

    def render(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        for model in self.models:
            model[1].use()
            uniform = model[1].uniforms.get("model")
            if not uniform is None:
                glDepthMask(GL_TRUE)
                glUniformMatrix4fv(uniform, 1, 1, model[2].data)
                model[0].render()
            else:
                glDepthMask(GL_FALSE)
                model[0].render()

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
    def direction(self):
        return self.camera.direction

    @direction.setter
    def direction(self, direction: Vec3):
        self.camera.direction = direction
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
