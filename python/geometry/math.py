import numbers
import numpy as np
import math


class Vec3:

    def __init__(self, x=None, y=None, z=None):
        self.data = np.array((0, 0, 0), dtype=np.float32)
        if isinstance(x, Vec3):
            self.data = np.copy(x.data)
        if isinstance(x, np.ndarray) and np.size(x) == 3:
            self.data = np.copy(x)
        elif isinstance(x, numbers.Number):
            if not (
                    isinstance(y, numbers.Number)
                    and isinstance(z, numbers.Number)):
                y = x
                z = x
            self.data = np.array((x, y, z), dtype=np.float32)

    def __add__(self, o: 'Vec3'):
        return Vec3(self.data + o.data)

    def __sub__(self, o: 'Vec3'):
        return Vec3(self.data - o.data)

    def __mul__(self, o: float):
        return Vec3(self.data * o)

    def __truediv__(self, o: float):
        o = 1.0 / o
        return self * o

    def __eq__(self, o: 'Vec3'):
        return all(self.data == o.data)

    def __str__(self):
        return "("+str(self.x) + ", " + str(self.y) + ", " + str(self.z) + ")"

    def dot(self, o: 'Vec3'):
        return sum(self.data * o.data)

    def cross(self, o: 'Vec3'):
        x = self.y*o.z - self.z*o.y
        y = self.z*o.x - self.x*o.z
        z = self.x*o.y - self.y*o.x
        return Vec3(x, y, z)

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

    def min(self, vec: 'Vec3'):
        x = min(self.x, vec.x)
        y = min(self.y, vec.y)
        z = min(self.z, vec.z)
        return Vec3(x, y, z)

    def max(self, vec: 'Vec3'):
        x = max(self.x, vec.x)
        y = max(self.y, vec.y)
        z = max(self.z, vec.z)
        return Vec3(x, y, z)

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


def distance(p0: Vec3, p1: Vec3):
    return (p0 - p1).norm()


def dot(v0, v1):
    return v0.dot(v1)


def cross(v0, v1):
    return v0.cross(v1)


def clamp(value, low=0.0, up=1.0):
    if (value < low):
        value = low
    if (value > up):
        value = up
    return value


def min(value0: float, value1: float):
    if (value0 < value1):
        return value0
    else:
        return value1


def max(value0: float, value1: float):
    if (value0 > value1):
        return value0
    else:
        return value1


def lerp(a, b, t):
    return a*(1-t) + b*t


def project_position_segment(p, a, b):
    ba = b - a
    t = (p - a).dot(ba) / ba.dot(ba)
    return lerp(a, b, clamp(t))


def project_segment_segment(a, b, c, d):
    dc = d-c
    dc_sqr_norm = dot(dc, dc)

    a_proj = a - (dc*(dot(a-c, dc)/dc_sqr_norm))
    b_proj = b - (dc*(dot(b-c, dc)/dc_sqr_norm))
    ba_proj = b_proj - a_proj
    t = dot(c-a_proj, ba_proj)/dot(ba_proj, ba_proj)
    if (a_proj == b_proj):
        t = 0.0

    ba_p = lerp(a, b, t)
    dc_p = project_position_segment(ba_p, c, d)
    ba_p = project_position_segment(dc_p, a, b)
    return (ba_p, dc_p)


class Mat3:

    def __init__(self, value=None):
        self.data = np.zeros((3, 3), dtype=np.float32)
        if isinstance(value, Mat3):
            self.data = np.copy(value.data)
        elif isinstance(value, np.ndarray) and np.size(value) == 9:
            self.data = np.copy(value)
        elif isinstance(value, list):
            if len(value) == 9:
                data = [[value[0], value[1], value[2]], [
                    value[3], value[4], value[5]], [value[6], value[7], value[8]]]
                self.data = np.array(data, dtype=np.float32)

    def __mul__(self, o):
        if isinstance(o, Vec3):
            return Vec3(np.matmul(self.data, o.data))
        if isinstance(o, Mat3):
            return Mat3(np.matmul(self.data, o.data))

    def __str__(self):
        return "(" + str(self.data[0][0]) + \
            ", " + str(self.data[0][1]) + ", " + str(self.data[0][2]) + ",\n" + str(self.data[1][0]) + \
            ", " + str(self.data[1][1]) + ", " + str(self.data[1][2]) + ",\n" + str(self.data[2][0]) + \
            ", " + str(self.data[2][1]) + ", " + str(self.data[2][2]) + ")"

    def identity(self):
        self.data = np.array(
            [[1, 0, 0], [0, 1, 0], [0, 0, 1]], dtype=np.float32)

    def transpose(self):
        return Mat3([self.data[0][0], self.data[1][0], self.data[2][0],
                     self.data[0][1], self.data[1][1], self.data[2][1],
                     self.data[0][2], self.data[1][2], self.data[2][2]])


def rotation_from_yaw_pitch(x: float, y: float):
    sin = math.sin(y)
    cos = math.cos(y)
    pitch = Mat3([1, 0, 0,
                  0, cos, -sin,
                  0, sin, cos])
    sin = math.sin(x)
    cos = math.cos(x)
    yaw = Mat3([cos, 0, sin,
                0, 1, 0,
                -sin, 0, cos])
    return yaw * pitch


def mat3_from_rotation(v: Vec3):
    sin = math.sin(v.y)
    cos = math.cos(v.y)
    ry = Mat3([1, 0, 0,
                  0, cos, -sin,
                  0, sin, cos])
    sin = math.sin(v.x)
    cos = math.cos(v.x)
    rx = Mat3([cos, 0, sin,
                0, 1, 0,
                -sin, 0, cos])
    sin = math.sin(v.z)
    cos = math.cos(v.z)
    rz = Mat3([cos, -sin, 0,
               sin, cos, 0,
               0, 0, 1])
    return rz * ry * rx


def mat4_from_rotation(v: Vec3):
    sin = math.sin(v.x)
    cos = math.cos(v.x)
    rx = Mat4([1, 0, 0, 0,
               0, cos, -sin, 0,
               0, sin, cos, 0, 
               0, 0, 0, 1])
    sin = math.sin(v.y)
    cos = math.cos(v.y)
    ry = Mat4([cos, 0, sin, 0,
               0, 1, 0, 0,
               -sin, 0, cos, 0,
               0, 0, 0, 1])
    sin = math.sin(v.z)
    cos = math.cos(v.z)
    rz = Mat4([cos, -sin, 0, 0,
               sin, cos, 0, 0,
               0, 0, 1, 0,
               0, 0, 0, 1])
    return rz * ry * rx


class Mat4:

    def __init__(self, value=None):
        self.data = np.zeros((4, 4), dtype=np.float32)
        if isinstance(value, Mat4):
            self.data = np.copy(value.data)
        elif isinstance(value, np.ndarray) and np.size(value) == 16:
            self.data = np.copy(value)
        elif isinstance(value, list):
            if len(value) == 16:
                data = [[value[0], value[1],  value[2], value[3]],
                        [value[4], value[5],  value[6], value[7]],
                        [value[8], value[9],  value[10], value[11]],
                        [value[12], value[13],  value[14], value[15]]]
                self.data = np.array(data, dtype=np.float32)

    def __mul__(self, o):
        if isinstance(o, Mat4):
            return Mat4(np.matmul(self.data, o.data))

    def __str__(self):
        return "(" + str(self.data[0][0]) + ", " + str(self.data[0][1]) + ", " + \
            str(self.data[0][2]) + ", " + str(self.data[0][3]) + ",\n" + \
            str(self.data[1][0]) + ", " + str(self.data[1][1]) + ", " + \
            str(self.data[1][2]) + ", " + str(self.data[1][3]) + ",\n" + \
            str(self.data[2][0]) + ", " + str(self.data[2][1]) + ", " + \
            str(self.data[2][2]) + ", " + str(self.data[2][3]) + ",\n" + \
            str(self.data[3][0]) + ", " + str(self.data[3][1]) + ", " + \
            str(self.data[3][2]) + ", " + str(self.data[3][3]) + ")"

    def identity(self):
        self.data = np.array([[1, 0, 0, 0], [0, 1, 0, 0],
                             [0, 0, 1, 0], [0, 0, 0, 1]], dtype=np.float32)

    def translate(self, v: Vec3):
        self.data[0,3] += v.x
        self.data[1, 3] += v.y
        self.data[2, 3] += v.z

