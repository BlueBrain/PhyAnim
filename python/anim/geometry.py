from platform import node
from render.mesh import *


class Node:

    def __init__(self, position: Vec3, radius: float = 1.0, id: int = 0,
                 fixed: bool = False, mass: float = 1.0, velocity: Vec3 = Vec3(),
                 force: Vec3 = Vec3()):
        self.position = position
        self.radius = radius
        self.id = id
        self.fixed = fixed
        self.mass = mass
        self.velocity = velocity
        self.force = force
        self.collide = False

    def update(self, dt: float):
        if not self.fixed:
            a = self.force / self.mass
            self.velocity += a * dt
            self.position += self.velocity * dt


class Section:
    def _tangents(self):
        num = len(self._nodes)
        tangents = []
        tangents.append(
            (self._nodes[1].position - self._nodes[0].position).normalized())
        for i in range(1, num-1):
            tangents.append(
                (self._nodes[i+1].position - self._nodes[i-1].position).normalized())
        tangents.append(
            (self._nodes[num-1].position - self._nodes[num-2].position).normalized())
        return tangents

    def _geometry(self):
        positions = []
        normals = []
        colors = []
        tangents = self._tangents()

        for i in range(len(self._nodes)):
            node = self._nodes[i]
            p = node.position
            r = node.radius
            t = tangents[i]

            if t.norm() == 0:
                t = Vec3(1, 0, 0)
            axis1 = Vec3(0, 0, 1)
            axis0 = (axis1.cross(t)).normalized()
            axis1 = (t.cross(axis0)).normalized()

            positions += [axis0 * r + p, axis1 * r + p,
                          axis0 * -r + p, axis1 * -r + p]
            normals += [axis0 * r, axis1 * r, axis0 * -r, axis1 * -r]
            if node.collide:
                colors += [self.colorCol] * 4
            else:
                colors += [self.colorNoCol] * 4

        return (positions, normals, colors)

    def _primitives(self):
        lines = []
        triangles = []
        quads = []
        for i in range(len(self._nodes) - 1):
            id0 = i * 4
            id1 = (i+1) * 4
            quads.append(Quad(id0, id1, id0+1, id1+1))
            quads.append(Quad(id0+1, id1+1, id0+2, id1+2))
            quads.append(Quad(id0+2, id1+2, id0+3, id1+3))
            quads.append(Quad(id0+3, id1+3, id0, id1))

        return (lines, triangles, quads)

    def __init__(self, nodes):
        self._nodes = nodes
        self.colorNoCol = Vec3(0.2, 0.8, 0.2)
        self.colorCol = Vec3(0.8, 0.2, 0.2)
        positions, normals, colors = self._geometry()
        lines, triangles, quads = self._primitives()

        self.mesh = Mesh(lines, triangles, quads, positions, normals, colors)

    def clearCollide(self):
        for node in self._nodes:
            node.collide = False

    def update(self):
        positions, normals, colors = self._geometry()
        self.mesh.update_positions(positions)
        self.mesh.update_normals(normals)
        self.mesh.update_colors(colors)


class Spring:

    def __init__(
            self, node0: Node, node1: Node, ks: float = 1000.0, kd: float = 0.1):
        self._node0 = node0
        self._node1 = node1
        self._ks = ks
        self._kd = kd
        self._lenght = (node1.position - node0.position).norm()
        if self._lenght == 0.0:
            self._lenght = 0.001

    @ property
    def initLength(self):
        return self._lenght

    @ property
    def id0(self):
        return self._node0.id

    @ property
    def id1(self):
        return self._node1.id

    def length(self):
        return distance(self._node0.position, self._node1.position)

    def add_forces(self):
        r = self._lenght
        pd = self._node1.position - self._node0.position
        l = pd.norm()
        vd = self._node1.velocity - self._node0.velocity
        force = Vec3()

        if l > 0.0:
            force = pd * (self._ks * (l / r - 1.0) +
                          self._kd * (vd.dot(pd) / (l * r))) / l

        self._node0.force += force
        self._node1.force -= force

    def collide(self, other: 'Spring'):
        p0 = (self._node0.position + self._node1.position) * 0.5
        p1 = (other._node0.position + other._node1.position) * 0.5

        r0 = (self._node0.position - self._node1.position).norm()
        r1 = (other._node0.position - other._node1.position).norm()

        if (p1-p0).norm() < r0 + r1:
            self._node0.collide = True
            self._node1.collide = True
            other._node0.collide = True
            other._node1.collide = True
