from geometry.math import *


class Line:

    def __init__(self, id0: int, id1: int):
        self._id0 = id0
        self._id1 = id1

    @property
    def id0(self):
        return self._id0

    @property
    def id1(self):
        return self._id1


class Triangle:

    def __init__(self, id0: int, id1: int, id2: int):
        self._id0 = id0
        self._id1 = id1
        self._id2 = id2

    @property
    def id0(self):
        return self._id0

    @property
    def id1(self):
        return self._id1

    @property
    def id2(self):
        return self._id2

    def __str__(self):
        return str(self.id0) + ", " + str(self.id1) + ", " + str(self.id2)


class Quad:

    def __init__(self, id0: int, id1: int, id2: int, id3: int):
        self._id0 = id0
        self._id1 = id1
        self._id2 = id2
        self._id3 = id3

    @property
    def id0(self):
        return self._id0

    @property
    def id1(self):
        return self._id1

    @property
    def id2(self):
        return self._id2

    @property
    def id3(self):
        return self._id3

    def __str__(self):
        return str(self.id0) + ", " + str(self.id1) + ", " + str(self.id2) + ", " + str(self.id3)


class Sphere:
    def __init__(self, position: vec3, radius: float):
        self.position = position
        self.radius = radius

    def get_geometry(self, color: vec3):
        axis_x = vec3(self.radius, 0, 0)
        axis_y = vec3(0, self.radius, 0)
        axis_z = vec3(0, 0, self.radius)

        positions = [self.position + axis_x, self.position - axis_x,
                     self.position + axis_y, self.position - axis_y,
                     self.position + axis_z, self.position - axis_z]
        normals = [axis_x, -axis_x, axis_y, -axis_y, axis_z, -axis_z]
        colors = [color, color, color, color, color, color]
        return (positions, normals, colors)

    def get_triangles(self, id: int = 0):
        triangles = [Triangle(id+2, id+4, id), Triangle(id+4, id+2, id+1),
                     Triangle(id+5, id+2, id), Triangle(id+2, id+5, id+1),
                     Triangle(id+3, id+5, id), Triangle(id+5, id+3, id+1),
                     Triangle(id+4, id+3, id), Triangle(id+3, id+4, id+1)]
        return triangles

    def get_quads(self, id: int = 0):
        quads = [Quad(id+2, id+4, id, id), Quad(id+4, id+2, id+1, id+1),
                 Quad(id+5, id+2, id, id), Quad(id+2, id+5, id+1, id+1),
                 Quad(id+3, id+5, id, id), Quad(id+5, id+3, id+1, id+1),
                 Quad(id+4, id+3, id, id), Quad(id+3, id+4, id+1, id+1)]
        return quads


class Capsule:
    def __init__(self, p0: vec3, r0: float, p1: vec3, r1: float):
        self.p0 = p0
        self.r0 = r0
        self.p1 = p1
        self.r1 = r1

    def get_geometry(self, color: vec3):
        axis_x = (self.p1 - self.p0).normalized()
        if axis_x.norm() == 0:
            axis_x = vec3(1, 0, 0)
        if axis_x.y == 1.0 or axis_x.y == -1.0:
            axis_y = vec3(-1, 0, 0)
        else:
            axis_y = vec3(0, 1, 0)
        axis_z = (axis_x.cross(axis_y)).normalized()
        axis_y = (axis_z.cross(axis_x)).normalized()

        positions = [axis_y * self.r0 + self.p0,
                     axis_z * self.r0 + self.p0,
                     axis_y * -self.r0 + self.p0,
                     axis_z * -self.r0 + self.p0,
                     axis_y * self.r1 + self.p1,
                     axis_z * self.r1 + self.p1,
                     axis_y * -self.r1 + self.p1,
                     axis_z * -self.r1 + self.p1,
                     axis_x * -self.r0 + self.p0,
                     axis_x * self.r1 + self.p1]
        normals = [axis_y * self.r0, axis_z * self.r0,
                   axis_y * -self.r0, axis_z * -self.r0,
                   axis_y * self.r1, axis_z * self.r1,
                   axis_y * -self.r1, axis_z * -self.r1,
                   axis_x * -self.r0, axis_x * self.r1]
        colors = [color, color, color, color, color,
                  color, color, color, color, color]
        return (positions, normals, colors)

    def get_triangles(self, id: int = 0):
        triangles = [Triangle(id, id+1, id+5), Triangle(id, id+5, id+4),
                     Triangle(id+1, id+2, id+6), Triangle(id+1, id+6, id+5),
                     Triangle(id+2, id+3, id+7), Triangle(id+2, id+7, id+6),
                     Triangle(id+3, id, id+4), Triangle(id+3, id+4, id+7),
                     Triangle(id+8, id+1, id), Triangle(id+8, id+2, id+1),
                     Triangle(id+8, id+3, id+2), Triangle(id+8, id, id+3),
                     Triangle(id+9, id+4, id+5), Triangle(id+9, id+5, id+6),
                     Triangle(id+9, id+6, id+7), Triangle(id+9, id+7, id+4)]
        return triangles

    def get_quads(self, id: int = 0):
        quads = [Quad(id, id+1, id+4, id+5), Quad(id+1, id+2, id+5, id+6),
                 Quad(id+2, id+3, id+6, id+7), Quad(id+3, id, id+7, id+4),
                 Quad(id+8, id+8, id, id+1), Quad(id+8, id+8, id+1, id+2),
                 Quad(id+8, id+8, id+2, id+3), Quad(id+8, id+8, id+3, id),
                 Quad(id+4, id+5, id+9, id+9), Quad(id+5, id+6, id+9, id+9),
                 Quad(id+6, id+7, id+9, id+9), Quad(id+7, id+4, id+9, id+9)]
        return quads


Segment = (int, int)
