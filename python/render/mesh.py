from __future__ import annotations
import time
from OpenGL.GL import *
from geometry.geometry import *
import numpy as np

def sort_element_x(element):
    return element.center().x

def sort_element_y(element):
    return element.center().y

def sort_element_z(element):
    return element.center().z

class AABoundingBox:

    def __init__(self):
        self.min = vec3(1e24)
        self.max = vec3(-1e24)
        self.childs = []
        self.elements = []

    def center(self):
        return (self.min + self.max) * 0.5

    def radius(self):
        if self.max.x < -1e23:
            return 1.0
        return length(self.max - self.center())

    def add_pos(self, position: vec3):
        self.min = min(self.min, position)
        self.max = max(self.max, position)

    def add_aabb(self, o: 'AABoundingBox', p: vec3 = vec3()):
        self.min = min(self.min, o.min + p)
        self.max = max(self.max, o.max + p)

    def update(self):
        self.min = vec3(1e24)
        self.max = vec3(-1e24)

        for child in self.childs:
            child.update()
            self.add_aabb(child)
        for element in self.elements:
            self.min = min(self.min,element.min())
            self.max = max(self.max,element.max())

    def divide(self):
        self.update()
        if len(self.elements) > 1:
            diff = abs(self.max - self.min)
            if diff.x > diff.y and diff.x > diff.z:
                self.elements.sort(key=sort_element_x)
            elif diff.y > diff.z:
                self.elements.sort(key=sort_element_y)
            else:
                self.elements.sort(key=sort_element_z)

            # self.elements.sort(key=sort_element_x)
            child0 = AABoundingBox()
            child1 = AABoundingBox()
            self.childs += [child0, child1]
            middle = len(self.elements) // 2
            child0.elements = self.elements[:middle]
            child1.elements = self.elements[middle:]
            self.elements = []
            child0.divide()
            child1.divide()

    def is_colliding(self, other: 'AABoundingBox'):
        return not (other.min.x > self.max.x or other.max.x < self.min.x or
                other.min.y > self.max.y or other.max.y < self.min.y or
                other.min.z > self.max.z or other.max.z < self.min.z)

    def is_inside(self, vec:vec3):
        return  not (vec.x < self.min.x or vec.x > self.max.x or
            vec.y < self.min.y or vec.y > self.max.y or 
            vec.z < self.min.z or vec.z > self.max.z)

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
                    pairs += other.colliding_pairs(child)
        return pairs
    
    def colliding_elements(self, other: 'AABoundingBox'):
        elements = []
        if self.is_colliding(other):
            if not self.childs:
                for element in self.elements:
                    if other.is_colliding(element.aabb()):
                        elements.append(element)
            else:
                for child in self.childs:
                    elements += self.colliding_elements(other)
        return elements

class Mesh:

    def __init__(self, lines, triangles, quads, positions, normals, colors):
        self.aabb = AABoundingBox()
        self.update_lines(lines)
        self.update_triangles(triangles)
        self.update_quads(quads)
        self.update_positions(positions, True)
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
        self.lines = np.empty([len(lines)*2], dtype=np.uint32)
        for i,l in enumerate(lines):
            self.lines[i*2] = l.id0
            self.lines[i*2+1] = l.id1
        self.num_lines = len(lines)*2
        self.__update_lines = True

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
            self.vbo_lines = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_lines)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        self.lines.nbytes, self.lines, GL_STATIC_DRAW)
            glBindVertexArray(0)
            self.lines = None

    def update_triangles(self, triangles):
        self.triangles = np.empty([len(triangles)*3], dtype=np.uint32)
        for i,t in enumerate(triangles):
            self.triangles[i*3] = t.id0
            self.triangles[i*3+1] = t.id1
            self.triangles[i*3+2] = t.id2
        self.num_triangles = len(triangles)*3
        self.__update_triangles = True

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
            self.vbo_triangles = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_triangles)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        self.triangles.nbytes, self.triangles, GL_STATIC_DRAW)
            glBindVertexArray(0)
            self.triangles = None

    def update_quads(self, quads):
        self.quads = np.empty([len(quads)*4], dtype=np.uint32)
        for i,q in enumerate(quads):
            self.quads[i*4] = q.id0
            self.quads[i*4+1] = q.id1
            self.quads[i*4+2] = q.id2
            self.quads[i*4+3] = q.id3
        self.num_quads = len(quads)*4
        self.__update_quads = True

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
            self.vbo_quads = glGenBuffers(1)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbo_quads)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        self.quads.nbytes, self.quads, GL_STATIC_DRAW)
            glBindVertexArray(0)
            self.quads = None

    def update_positions(self, positions, compute_aabb = False):
        prev = time.time()
        if compute_aabb:
            self.aabb = AABoundingBox()
            for position in positions:
                self.aabb.add_pos(position)

        self.positions = np.empty([len(positions)*3], dtype=np.float32)
        for i,position in enumerate(positions):
            self.positions[i*3] = position.x
            self.positions[i*3+1] =  position.y
            self.positions[i*3+2] = position.z
        self.__update_positions = True
        # print("\n-Time to update {:.6f}".format(time.time()-prev))

    def _update_positions(self):
        self.__update_positions = False
        if len(self.positions) > 0:
            if self.vbo_position < 0:
                self.vbo_position = glGenBuffers(1)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_position)
            glBufferData(GL_ARRAY_BUFFER, self.positions.nbytes,
                         self.positions, GL_STATIC_DRAW)
            self.positions = None

    def update_normals(self, normals):
        self.normals = np.empty([len(normals)*3], dtype=np.float32)
        for i,normal in enumerate(normals):
            self.normals[i*3] = normal.x 
            self.normals[i*3+1] = normal.y
            self.normals[i*3+2] = normal.z
        self.__update_normals = True
    
    def _update_normals(self):
        self.__update_normals = False
        if len(self.normals) > 0:
            if self.vbo_normal < 0:
                self.vbo_normal = glGenBuffers(1)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_normal)
            glBufferData(GL_ARRAY_BUFFER, self.normals.nbytes,
                         self.normals, GL_STATIC_DRAW)
            self.normals = None

    def update_colors(self, colors):
        self.colors = np.empty([len(colors)*3], dtype=np.float32)
        for i, color in enumerate(colors):
            self.colors[i*3] = color.x
            self.colors[i*3+1] = color.y
            self.colors[i*3+2] = color.z
        self.__update_colors = True
 
    def _update_colors(self):
        self.__update_colors = False
        if len(self.colors) > 0:
            if self.vbo_color < 0:
                self.vbo_color = glGenBuffers(1)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo_color)
            glBufferData(GL_ARRAY_BUFFER, self.colors.nbytes,
                         self.colors, GL_STATIC_DRAW)
            self.colors = None

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
        positions = [vec3(-1, 1, 0.0), vec3(-1, -1, 0.0),
                     vec3(1, -1, 0)]
        normals = [vec3(0, 0, 1), vec3(0, 0, 1),
                   vec3(0, 0, 1)]
        colors = [vec3(1, 0, 0), vec3(0, 1, 0),
                  vec3(0, 0, 1)]
        Mesh.__init__(self, lines, triangles, quads,
                      positions, normals, colors)

class QuadMesh(Mesh):
    def __init__(self):
        lines = []
        triangles = [Triangle(0, 1, 2), Triangle(0, 2, 3)]
        quads = [Quad(0, 1, 3, 2)]
        positions = [vec3(-1, 1, 0.0), vec3(-1, -1, 0.0),
                     vec3(1, -1, 0), vec3(1, 1, 0.0)]
        normals = [vec3(0, 0, 1), vec3(0, 0, 1),
                   vec3(0, 0, 1), vec3(0, 0, 1)]
        # colors = [vec3(0.9, 0.9, 0.9), vec3(1, 1, 1),
        #           vec3(1, 1, 1), vec3(0.9, 0.9, 0.9)]
        colors = [vec3(0.2, 0.2, 0.2), vec3(0.1, 0.2, 0.2),
                  vec3(0.1, 0.2, 0.2), vec3(0.2, 0.2, 0.2)]
        Mesh.__init__(self, lines, triangles, quads,
                      positions, normals, colors)

class SphereMesh(Mesh):
    def __init__(self):
        color = vec3(0.8, 0.8, 1)
        s = Sphere(vec3(), 1.0)
        (positions, normals, colors) = s.get_geometry(color)
        triangles = s.get_triangles()
        quads = s.get_quads()
        Mesh.__init__(self, [], triangles, quads,
                      positions, normals, colors)

class CapsuleMesh(Mesh):
    def __init__(self, p0: vec3, r0: float, p1: vec3, r1: float):
        color = vec3(0.8, 1, 1)
        c = Capsule(p0, r0, p1, r1)
        (positions, normals, colors) = c.get_geometry(color)
        triangles = c.get_triangles()
        quads = c.get_quads()
        Mesh.__init__(self, [], triangles, quads,
                      positions, normals, colors)

class CubeMesh(Mesh):
    def __init__(self, min:vec3, max:vec3):
        lines = [Line(0, 1), Line(0, 2), Line(1, 3), Line(2, 3),
                 Line(4, 5), Line(4, 6), Line(5, 7), Line(6, 7),
                 Line(0, 4), Line(1, 5), Line(2, 6), Line(3, 7)]
        triangles = []
        quads = []
        positions = [vec3(min.x, min.y, min.z), vec3(max.x, min.y, min.z),
                     vec3(min.x, max.y, min.z), vec3(max.x, max.y, min.z),
                     vec3(min.x, min.y, max.z), vec3(max.x, min.y, max.z),
                     vec3(min.x, max.y, max.z), vec3(max.x, max.y, max.z)]
        normals = []
        colors = [vec3(1, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0),
                  vec3(0,1,0), vec3(0,1,0), vec3(0,1,0), vec3(0,1,0)]
        Mesh.__init__(self, lines, triangles, quads,
                      positions, normals, colors)
