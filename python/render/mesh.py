from __future__ import annotations
from OpenGL.GL import *
from geometry.geometry import *

def sort_element_x(element):
    return element.center().x

def sort_element_y(element):
    return element.center().y

def sort_element_z(element):
    return element.center().z

class AABoundingBox:

    def __init__(self):
        self.min = Vec3(1e24)
        self.max = Vec3(-1e24)
        self.childs = []
        self.elements = []

    def center(self):
        return (self.min + self.max) * 0.5

    def radius(self):
        if self.max.x < -1e23:
            return 1.0
        return (self.max - self.center()).norm()

    def add_pos(self, position: Vec3):
        self.min = self.min.min(position)
        self.max = self.max.max(position)

    def add_aabb(self, o: 'AABoundingBox', p:Vec3 = Vec3()):
        self.min = self.min.min(o.min + p)
        self.max = self.max.max(o.max + p)

    def update(self):
        self.min = Vec3(1e24)
        self.max = Vec3(-1e24)

        for child in self.childs:
            child.update()
            self.add_aabb(child)
        for element in self.elements:
            self.min = self.min.min(element.min())
            self.max = self.max.max(element.max())

    def divide(self):
        self.update()
        if len(self.elements) > 2:
            diff = self.max - self.min
            if diff.x > diff.y and diff.x > diff.z:
                self.elements.sort(key=sort_element_x)
            elif diff.y > diff.z:
                self.elements.sort(key=sort_element_y)
            else:
                self.elements.sort(key=sort_element_z)
            child0 = AABoundingBox()
            child1 = AABoundingBox()
            self.childs += [child0, child1]
            middle = len(self.elements) // 2
            child0.elements += self.elements[:middle]
            child1.elements += self.elements[middle:]
            child0.divide()
            child1.divide()
            self.elements = []
            self.update()
            
            # for i,ele in enumerate(self.elements):
            #     print(centers[i].y, center.y)
            #     if centers[i].y < center.y:
            #         print("left")
            #         child0.elements.append(ele)
            #     else:
            #         print("right")
            #         child1.elements.append(ele)
           

    def is_colliding(self, other: 'AABoundingBox'):
        collide = True
        if (other.min.x > self.max.x or other.max.x < self.min.x or
                other.min.y > self.max.y or other.max.y < self.min.y or
                other.min.z > self.max.z or other.max.z < self.min.z):
            collide = False
        return collide

    def colliding_pairs(self, other: 'AABoundingBox'):
        pairs = []
        if self.is_colliding(other):
            if not self.childs and not other.childs:
                for element0 in self.elements:
                    for element1 in other.elements:
                        pairs.append((element0, element1))
            elif not self.childs:
                for o_child in other.childs:
                    pairs += self.colliding_pairs(o_child)
            else:
                for child in self.childs:
                    for o_child in other.childs:
                        pairs += child.colliding_pairs(o_child)
        return pairs

class Mesh:

    def __init__(self, lines, triangles, quads, positions, normals, colors):
        self.aabb = AABoundingBox()
        self.update_lines(lines)
        self.update_triangles(triangles)
        self.update_quads(quads)
        self.update_positions(positions)
        self.update_normals(normals)
        self.update_colors(colors)
        self.vbo_position = -1
        self.vbo_normal = -1
        self.vbo_color = -1
        self.vbo_lines = -1
        self.vbo_triangles = -1
        self.vbo_quads = -1
        self.vao_lines = -1
        self.vao_triangles = -1
        self.vao_quads = -1

        self.__update_lines = True
        self.__update_triangles = True
        self.__update_quads = True
        self.__update_positions = True
        self.__update_normals = True
        self.__update_colors = True

    def check_update(self): 
        if self.__update_positions:
            self._update_positions()
        if self.__update_normals:
            self._update_normals()
        if self.__update_colors:
            self._update_colors()
        if self.__update_lines:
            self._update_lines()
        if self.__update_triangles:
            self._update_triangles()
        if self.__update_quads:
            self._update_quads()
        

    def update_lines(self, lines):
        self.lines = []
        for l in lines:
            self.lines += [l.id0, l.id1]
        self.num_lines = len(lines)*2
        self.__update_lines = True

    def update_triangles(self, triangles):
        self.triangles = []
        for t in triangles:
            self.triangles += [t.id0, t.id1, t.id2]
        self.num_triangles = len(triangles)*3
        self.__update_triangles = True

    def update_quads(self, quads):
        self.quads = []
        for q in quads:
            self.quads += [q.id0, q.id1, q.id2, q.id3]
        self.num_quads = len(quads)*4
        self.__update_quads = True

    def update_positions(self, positions):
        self.positions = []
        for position in positions:
            self.aabb.add_pos(position)
            self.positions.append([position.x, position.y, position.z])
        self.__update_positions = True
    
    def update_normals(self, normals):
        self.normals = []
        for normal in normals:
            self.normals.append([normal.x, normal.y, normal.z])
        self.__update_normals = True
    
    def update_colors(self, colors):
        self.colors = []
        for color in colors:
            self.colors.append([color.x, color.y, color.z])
        self.__update_colors = True

    def _update_lines(self):
        self.__update_lines = False
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
            lines_vec = np.array(self.lines, dtype=np.uint32)
            self.vbo_lines = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_lines)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        lines_vec.nbytes, lines_vec, GL_STATIC_DRAW)
            glBindVertexArray(0)
            self.lines = None

    def _update_triangles(self):
        self.__update_triangles = False
        self.num_triangles = len(self.triangles)*3
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
            triangles_vec = np.array(self.triangles, dtype=np.uint32)
            self.vbo_triangles = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_triangles)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        triangles_vec.nbytes, triangles_vec, GL_STATIC_DRAW)
            glBindVertexArray(0)
            self.triangles = None

    def _update_quads(self):
        self.__update_quads = False
        self.num_quads = len(self.quads)*4
        if self.num_quads > 0:
            
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
            quads_vec = np.array(self.quads, dtype=np.uint32)
            self.vbo_quads = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_quads)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        quads_vec.nbytes, quads_vec, GL_STATIC_DRAW)
            glBindVertexArray(0)

    def __enter__(self) -> Mesh:
        return self
    
    def __exit__(self, *_) -> None:
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

    def _update_positions(self):
        self.__update_positions = False
        if len(self.positions) > 0:
            if self.vbo_position < 0:
                self.vbo_position = glGenBuffers(1)
            positions_vec = np.array(self.positions, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_position)
            glBufferData(GL_ARRAY_BUFFER, positions_vec.nbytes,
                         positions_vec, GL_STATIC_DRAW)

    def _update_normals(self):
        self.__update_normals = False
        if len(self.normals) > 0:
            if self.vbo_normal < 0:
                self.vbo_normal = glGenBuffers(1)
            normals_vec = np.array(self.normals, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_normal)
            glBufferData(GL_ARRAY_BUFFER, normals_vec.nbytes,
                         normals_vec, GL_STATIC_DRAW)

    def _update_colors(self):
        self.__update_colors = False
        if len(self.colors) > 0:
            if self.vbo_color < 0:
                self.vbo_color = glGenBuffers(1)
            colors_vec = np.array(self.colors, dtype=np.float32)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_color)
            glBufferData(GL_ARRAY_BUFFER, colors_vec.nbytes,
                         colors_vec, GL_STATIC_DRAW)

    def render_lines(self):
        self.check_update()
        if self.vao_lines >= 0:
            glBindVertexArray(self.vao_lines)
            glDrawElements(GL_LINES, self.num_lines,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))

    def render_triangles(self):
        self.check_update()
        if self.vao_triangles >= 0:
            glBindVertexArray(self.vao_triangles)
            glDrawElements(GL_TRIANGLES, self.num_triangles,
                           GL_UNSIGNED_INT, ctypes.c_void_p(0))

    def render_patches(self):
        self.check_update()
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
