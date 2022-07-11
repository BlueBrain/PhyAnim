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
            if not (isinstance(y, numbers.Number) and isinstance(z, numbers.Number)):
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

    def vec(self):
        return [self.x, self.y, self.z]

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


class Mat3:

    def __init__(self, value=None):
        self.data = np.zeros((3, 3), dtype=np.float32)
        if isinstance(value, Mat3):
            self.data = np.copy(value.data)
        elif isinstance(value, np.ndarray) and np.size(value) == 9:
            self.data = np.copy(value)
        elif isinstance(value, list):
            if len(value) == 9:
                data = [[value[0], value[1],  value[2]],
                        [value[3], value[4],  value[5]], [value[6], value[7],  value[8]]]
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
