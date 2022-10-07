import sys
from OpenGL.GL import *
from geometry.geometry import *


class AABoundingBox:

    def __init__(self):
        self.min = Vec3(sys.float_info.max)
        self.max = Vec3(sys.float_info.min)
        self.childs = []
        self.elements = []

    def center(self):
        return (self.min + self.max) * 0.5

    def radius(self):
        return (self.max - self.center()).norm()

    def add_pos(self, position: Vec3):
        self.min = self.min.min(position)
        self.max = self.max.max(position)

    def update(self):
        self.min = Vec3(sys.float_info.max)
        self.max = Vec3(sys.float_info.min)

        for child in self.childs:
            child.update()
            self.min = self.min.min(child.min)
            self.max = self.max.max(child.max)
        for element in self.elements:
            self.min = self.min.min(element.min())
            self.max = self.max.max(element.max())

    def is_colliding(self, other: 'AABoundingBox'):

        collide = True

        if (other.min.x > self.max.x or other.max.x < self.min.x or
                other.min.y > self.max.y or other.max.y < self.min.y or
                other.min.z > self.max.z or other.max.z < self.min.z):
            collide = False

        return collide


class Mesh:

    def __init__(self, lines, triangles, quads, positions, normals, colors):

        self.vbo_position = -1
        self.vbo_normal = -1
        self.vbo_color = -1
        self.vbo_lines = -1
        self.vbo_triangles = -1
        self.vbo_quads = -1
        self.vao_lines = -1
        self.vao_triangles = -1
        self.vao_quads = -1
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

        # Quads
        self.num_quads = len(quads)*4
        if self.num_quads > 0:
            quads_vec = []
            for q in quads:
                quads_vec += [q.id0, q.id1, q.id2, q.id3]
            self.vao_quads = glGenVertexArrays(1)
            glBindVertexArray(self.vao_quads)

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

            quads_vec = np.array(quads_vec, dtype=np.uint32)
            self.vbo_quads = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_quads)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         quads_vec.nbytes, quads_vec, GL_STATIC_DRAW)
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
        if (self.vbo_quads >= 0):
            glDeleteBuffers(1, self.vbo_quads)
        if (self.vao_lines >= 0):
            glDeleteVertexArrays(1, self.vao_lines)
        if (self.vao_triangles >= 0):
            glDeleteVertexArrays(1, self.vao_triangles)
        if (self.vao_quads >= 0):
            glDeleteVertexArrays(1, self.vao_quads)

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
        if self.vao_quads >= 0:
            glPatchParameteri(GL_PATCH_VERTICES, 4)
            glBindVertexArray(self.vao_quads)
            glDrawElements(GL_PATCHES, self.num_quads,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))


class TriangleMesh(Mesh):
    def __init__(self):
        lines = []
        triangles = [Triangle(0, 1, 2)]
        quads = []
        positions = [Vec3(-1, 1, 0.0), Vec3(-1, -1, 0.0),
                     Vec3(1, -1, 0)]
        normals = [Vec3(0, 0, 1), Vec3(0, 0, 1),
                   Vec3(0, 0, 1)]
        colors = [Vec3(1, 0, 0), Vec3(0, 1, 0),
                  Vec3(0, 0, 1)]
        Mesh.__init__(self, lines, triangles, quads,
                      positions, normals, colors)


class QuadMesh(Mesh):
    def __init__(self):
        lines = []
        triangles = [Triangle(0, 1, 2), Triangle(0, 2, 3)]
        quads = [Quad(0, 1, 3, 2)]
        positions = [Vec3(-1, 1, 0.0), Vec3(-1, -1, 0.0),
                     Vec3(1, -1, 0), Vec3(1, 1, 0.0)]
        normals = [Vec3(0, 0, 1), Vec3(0, 0, 1),
                   Vec3(0, 0, 1), Vec3(0, 0, 1)]
        colors = [Vec3(1, 1, 1), Vec3(0.6, 0.6, 1),
                  Vec3(0.6, 0.6, 1), Vec3(1, 1, 1)]
        Mesh.__init__(self, lines, triangles, quads,
                      positions, normals, colors)


class SphereMesh(Mesh):
    def __init__(self):
        color = Vec3(0.8, 0.8, 1)
        s = Sphere(Vec3(), 1.0)
        (positions, normals, colors) = s.get_geometry(color)
        triangles = s.get_triangles()
        quads = s.get_quads()
        Mesh.__init__(self, [], triangles, quads,
                      positions, normals, colors)


class CapsuleMesh(Mesh):
    def __init__(self, p0: Vec3, r0: float, p1: Vec3, r1: float):
        color = Vec3(0.8, 1, 1)
        c = Capsule(p0, r0, p1, r1)
        (positions, normals, colors) = c.get_geometry(color)
        triangles = c.get_triangles()
        quads = c.get_quads()
        Mesh.__init__(self, [], triangles, quads,
                      positions, normals, colors)
