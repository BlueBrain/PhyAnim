from array import array
import string
from turtle import position
from OpenGL.GL import *
from OpenGL.GL.shaders import *
from OpenGL_accelerate import *

from render.math import *


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
                      [-1, 1, 0.0, -1, -1, 0.999, 1, -1, 0.999, 1, 1, 0.999],
                      [0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1],
                      [1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1])


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

    def get_uniforms(self):
        return self.uniforms

    def use(self):
        if not self.id is None:
            glUseProgram(self.id)


class Camera:

    def __init__(self, position: Vec3, target: Vec3 = Vec3(), up: Vec3 = Vec3(0, 1, 0), near: float = 0.01, far: float = 1000.0, fov: float = 90.0, ratio: float = 1.0):
        self.position = position
        self.target = target
        self.up = up
        self.near = near
        self.far = far
        self.fov = fov * math.pi / 360.0
        self.ratio = ratio

        self.__build_view()
        self.__build_proj()

    def __build_view(self):
        self.view = Mat4()
        self.view.identity()
        self.view.data[0, 3] = -self.position.x
        self.view.data[1, 3] = -self.position.y
        self.view.data[2, 3] = -self.position.z

    def __build_proj(self):
        nf = 1.0 / (self.near - self.far)
        f = 1.0 / math.tan(self.fov)
        data = [f / self.ratio, 0.0, 0.0, 0.0,
                0.0, f, 0.0, 0.0,
                0.0, 0.0, (self.far + self.near) * nf, -1.0,
                0.0, 0.0, 2.0 * self.far * self.near * nf, 0.0]
        self.proj = Mat4(data)

    @property
    def position(self):
        return self._position

    @position.setter
    def position(self, value: Vec3):
        self._position = value
        self.__build_view()

    @property
    def ratio(self):
        return self._ratio

    @ratio.setter
    def ratio(self, value: float):
        self._ratio = value
        self.__build_proj()


class Scene:
    def __init__(self):
        self.program = ShaderProgram(
            [("shaders/scene.vert", ShaderType.VERTEX), ("shaders/scene.frag", ShaderType.FRAGMENT)])
        self.uniforms = self.program.get_uniforms()
        self.camera = Camera(Vec3(0, 0, 2))
        self.background_color = Vec3(1.0)
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)
        self.color = Vec3(0.2, 1.0, 1.0)
        self.meshes = []
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

    def render(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        self.program.use()
        glUniform3fv(self.uniforms["color"], 1, self.color.data)
        glUniformMatrix4fv(
            self.uniforms["uView"], 1, 0, self.camera.view.data)
        glUniformMatrix4fv(self.uniforms["proj"], 1, 0, self.camera.proj.data)

        for mesh in self.meshes:
            if not mesh is None:
                mesh.render()


class Engine:

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.background_color = Vec3(1.0)
        self.scene = Scene()
        self.mode = GL_FILL

    def resize(self, width, height):
        self.width = int(width)
        self.height = int(height)
        self.scene.camera.ratio = width * 1.0 / height

    @ property
    def scene(self):
        return self._scene

    @ scene.setter
    def scene(self, value):
        self._scene = value

    def render(self, t=0):
        self.scene.render()

    def render_mode(self):
        if self.mode == GL_FILL:
            self.mode = GL_LINE
        else:
            self.mode = GL_FILL
        glPolygonMode(GL_FRONT_AND_BACK, self.mode)


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
            if len(words) == 0:
                continue
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

    # num_vertex = int(len(positions)/3)
    # num_triangles = int(len(triangles)/3)

    # comp_normals = []
    # weights = []
    # for i in range(num_vertex):
    #     comp_normals.append(Vec3(0, 0, 0))
    #     weights.append(0.0)

    # for i in range(num_triangles):
    #     id0 = triangles[i*3]
    #     id1 = triangles[i*3+1]
    #     id2 = triangles[i*3+2]

    #     p0 = Vec3(positions[id0*3], positions[id0*3+1], positions[id0*3+2])
    #     p1 = Vec3(positions[id1*3], positions[id1*3+1], positions[id1*3+2])
    #     p2 = Vec3(positions[id2*3], positions[id2*3+1], positions[id2*3+2])
    #     axis0 = (p1-p0).normalized()
    #     axis1 = (p2-p0).normalized()
    #     normal = axis0.cross(axis1)
    #     comp_normals[id0] += normal
    #     comp_normals[id1] += normal
    #     comp_normals[id2] += normal
    #     weights[id0] += 1
    #     weights[id1] += 1
    #     weights[id2] += 1

    # normals = []
    # for i in range(num_vertex):
    #     comp_normals[i] /= weights[i]
    #     normals += [comp_normals[i].x, comp_normals[i].y, comp_normals[i].z]

    print("mesh loaded")
    return Mesh(lines, triangles, positions, normals, colors)
