from array import array
import math
import numbers
from OpenGL.GL import *
from OpenGL_accelerate import *
import numpy as np


class Vec3:

    def __init__(self, x=None, y=None, z=None):
        self.data = np.array((0,0,0), dtype=np.float32)
        if isinstance(x, Vec3):
            self.data = np.copy(x.data)
        if isinstance(x, np.ndarray) and np.size(x) == 3:
                self.data = np.copy(x)
        elif isinstance(x, numbers.Number):
            if not (isinstance(y, numbers.Number) and isinstance(z, numbers.Number)):
                y = x
                z = x
            self.data = np.array((x,y,z), dtype=np.float32)
        
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

    def __init__(self, vertices, indices):
        self.num_vertex = len(vertices)/6
        vertices = np.array(vertices, dtype = np.float32)
        self.num_index = len(indices)
        indices = np.array(indices, dtype = np.uint32)
        
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)
        self.vbos = glGenBuffers(2)
        
        glBindBuffer(GL_ARRAY_BUFFER, self.vbos[0])
        glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)
        
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, ctypes.c_void_p(0))
        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, ctypes.c_void_p(12))

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.vbos[1])
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.nbytes, indices, GL_STATIC_DRAW)

        glBindVertexArray(0)

    def __del__(self):
        glDeleteBuffers(2, self.vbos)
        glDeleteVertexArrays(1, self.vao)

    def render(self):
        glBindVertexArray(self.vao)
        glDrawElements(GL_LINES, self.num_index, GL_UNSIGNED_INT, ctypes.c_void_p(0))
    

class Engine:

    def __init__(self, width, height, background_color):
        self.width = width
        self.height = height
        self.background_color = background_color.normalized()
        glClearColor(self.background_color.x,
                     self.background_color.y, self.background_color.z, 1.0)

        vertices = [0, 0, 0, 1, 0, 0, 
                    1, 0 , 0, 0, 0 , 1]
        indices = [0,1]
        self.mesh = Mesh(vertices, indices)

    def render(self):
        glClear(GL_COLOR_BUFFER_BIT)
        self.mesh.render()
