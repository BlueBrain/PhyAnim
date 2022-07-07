from array import array
import math
import numbers
from OpenGL.GL import *
from OpenGL.GL.shaders import *
from OpenGL_accelerate import *

import numpy as np


class Vec3:

    def __init__(self, x=None, y=None, z=None):
        self.data = np.array((0, 0, 0), dtype=np.float32)
        if isinstance(x, Vec3):
            self.data = np.copy(x.data)
        if isinstance(x, np.ndarray) and np.size(x) == 3:
            self.data = np.copy(x)
        elif isinstance(x, numbers.Number):
            if not (isinstance(y, numbers.Number) and isinstance(z, numbers.Number)):
                y = x
                z = x
            self.data = np.array((x, y, z), dtype=np.float32)

    def __add__(self, o):
        if not isinstance(o, Vec3):
            raise "Invalid operation: Vec3 type is needed"
        return Vec3(self.data + o.data)

    def __sub__(self, o):
        if not isinstance(o, Vec3):
            raise "Invalid operation: Vec3 type is needed"
        return Vec3(self.data - o.data)

    def __mul__(self, o):
        if not isinstance(o, numbers.Number):
            raise "Invalid operation: numeric type is needed"
        return Vec3(self.data * o)

    def __truediv__(self, o):
        if not isinstance(o, numbers.Number):
            raise "Invalid operation: numeric type is needed"
        o = 1.0 / o
        return self * o

    def __eq__(self, o):
        return all(self.data == o.data)

    def __str__(self):
        return "("+str(self.x) + ", " + str(self.y) + ", " + str(self.z) + ")"

    def dot(self, o):
        if not isinstance(o, Vec3):
            raise "Invalid operation: Vec3 type is needed"
        return sum(self.data * o.data)

    def norm(self):
        return math.sqrt(self.dot(self))

    def normalize(self):
        normalized = self.normalized()
        self.data = normalized.data

    def normalized(self):
        norm = self.norm()
        if norm > 0:
            return self / norm
        return Vec3()

    def data(self):
        return np.copy(self.data)

    @property
    def x(self):
        return self.data[0]

    @x.setter
    def x(self, value):
        if not isinstance(value, numbers.Number):
            raise "Invalid assing: the value must be a number"
        self.data[0] = value

    @property
    def y(self):
        return self.data[1]

    @y.setter
    def y(self, value):
        if not isinstance(value, numbers.Number):
            raise "Invalid assing: the value must be a number"
        self.data[1] = value

    @property
    def z(self):
        return self.data[2]

    @z.setter
    def z(self, value):
        if not isinstance(value, numbers.Number):
            raise "Invalid assing: the value must be a number"
        self.data[2] = value


class Mesh:

    def __init__(self, lines, triangles, positions, normals, colors):

        self.vbo_position = -1
        self.vbo_normal = -1
        self.vbo_color = -1
        self.vbo_lines = -1
        self.vbo_triangles = -1
        self.vao_lines = -1
        self.vao_triangles = -1

        # Vertices
        self.vbo_position = glGenBuffers(1)
        positions = np.array(positions, dtype=np.float32)
        glBindBuffer(GL_ARRAY_BUFFER, self.vbo_position)
        glBufferData(GL_ARRAY_BUFFER, positions.nbytes,
                     positions, GL_STATIC_DRAW)
        if len(normals) > 0:
            self.vbo_normal = glGenBuffers(1)
            normals = np.array(normals, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_normal)
            glBufferData(GL_ARRAY_BUFFER, normals.nbytes,
                         normals, GL_STATIC_DRAW)
        if len(colors) > 0:
            self.vbo_color = glGenBuffers(1)
            colors = np.array(colors, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_color)
            glBufferData(GL_ARRAY_BUFFER, colors.nbytes,
                         colors, GL_STATIC_DRAW)

        # Lines
        self.num_lines = len(lines)
        if self.num_lines > 0:
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

            lines = np.array(lines, dtype=np.uint32)
            self.vbo_lines = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_lines)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         lines.nbytes, lines, GL_STATIC_DRAW)
            glBindVertexArray(0)

        # Triangles
        self.num_triangles = len(triangles)
        if self.num_triangles > 0:
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

            triangles = np.array(triangles, dtype=np.uint32)
            self.vbo_triangles = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_triangles)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         triangles.nbytes, triangles, GL_STATIC_DRAW)
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
        # print("Mesh deleted")

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
        Mesh.__init__(self, [], [0, 1, 2, 0, 2, 3],
                      [-1, 1, 0.999, -1, -1, 0.999, 1, -1, 0.999, 1, 1, 0.999], [0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1], [1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1])


class ShaderProgram:

    def __init__(self, shader_paths):
        self.id = self.__create_program(shader_paths[0], shader_paths[1])

    def __del__(self):
        glDeleteProgram(self.id)
        glDeleteShader(self.vertex_shader)
        glDeleteShader(self.fragment_shader)

    def __create_shader(self, shader_path, shader_type):
        with open(shader_path, 'r') as f:
            shader_source = f.readlines()
        return compileShader(shader_source, shader_type)

    def __create_program(self, vertex_path, fragment_path):
        self.vertex_shader = self.__create_shader(
            vertex_path, GL_VERTEX_SHADER)
        self.fragment_shader = self.__create_shader(
            fragment_path, GL_FRAGMENT_SHADER)
        return compileProgram(self.vertex_shader, self.fragment_shader)

    def use(self):
        glUseProgram(self.id)


class Scene:
    def __init__(self):
        self.program = ShaderProgram(
            ("shaders/scene.vert", "shaders/scene.frag"))
        self.meshes = []

    @property
    def meshes(self):
        return self._meshes

    @meshes.setter
    def meshes(self, value):
        self._meshes = value

    def render(self):
        self.program.use()
        for mesh in self.meshes:
            if not mesh is None:
                mesh.render()


class Engine:

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.background_color = Vec3(1.0)
        self.scene = Scene()
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)
        glEnable(GL_CULL_FACE)
        glEnable(GL_DEPTH_TEST)

    def resize(self, width, height):
        self.width = width
        self.height = height
        glViewport(0, 0, width, height)

    @property
    def background_color(self):
        return self._background_color

    @background_color.setter
    def background_color(self, value):
        self._background_color = value
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)

    @property
    def scene(self):
        return self._scene

    @scene.setter
    def scene(self, value):
        self._scene = value

    def render(self, t=0):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.scene.render()
        pass


def load_obj(path):

    positions = []
    normals = []
    colors = []
    lines = []
    triangles = []

    with open(path, 'r') as f:
        readlines = f.readlines()
        for line in readlines:
            words = line.split()
            if words[0] == 'v':
                positions += [float(words[1]),
                              float(words[2]), float(words[3])]
            elif words[0] == 'vn':
                normals += [float(words[1]),
                            float(words[2]), float(words[3])]
            elif words[0] == 'f':
                triangles += [int(words[1].split('/')[0])-1,
                              int(words[2].split('/')[0])-1,
                              int(words[3].split('/')[0])-1]

    for i in range(int(len(positions)/3)):
        colors += [1, 0, 0]

    return Mesh(lines, triangles, positions, normals, colors)
