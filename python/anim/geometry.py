from mailbox import NoSuchMailboxError
from platform import node

from render.mesh import *
import morphio.mut 

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

    def clear(self):
        self.collide = False
        self.force = Vec3()


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

    def min(self):
        return (self._node0.position - Vec3(self._node0.radius)).min(self._node1.position - Vec3(self._node1.radius))

    def max(self):
        return (self._node0.position + Vec3(self._node0.radius)).max(self._node1.position + Vec3(self._node1.radius))

    def center(self):
        return (self._node0.position + self._node1.position)*0.5

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

class Section:
    def __init__(self):
        self.children = []
        self.nodes = []

    def add_node(self, node: Node):
        self.nodes.append(node)

    def new_child(self):
        section = Section()
        self.children.append(section)
        section.add_node(self.nodes[-1])
        return section

    def geometry(self):
        positions = []
        normals = []
        colors = []
        color_collision = Vec3(0.8, 0.2, 0.2)
        color_no_collision = Vec3(0.2, 0.8, 0.2)
        for i,node in enumerate(self.nodes):
            p = node.position
            r = node.radius
            t = Vec3(1,0,0)
            if i == len(self.nodes)-1:
                node1 = self.nodes[i-1]
                t = (node.position - node1.position).normalized()
            else:
                node1 = self.nodes[i+1]
                t = (node1.position - node.position).normalized()
            if t.norm() == 0:
                t = Vec3(1, 0, 0)
            axis1 = Vec3(0, 0, 1)
            axis0 = (axis1.cross(t)).normalized()
            axis1 = (t.cross(axis0)).normalized()
            positions += [axis0 * r + p, axis1 * r + p, axis0 * -r + p, axis1 * -r + p]
            normals += [axis0 * r, axis1 * r, axis0 * -r, axis1 * -r]
            if node.collide:
                colors += [color_collision] * 4
            else:
                colors += [color_no_collision] * 4
        return (positions, normals, colors)

    def quads(self, id = 0):
        quads = []
        for i in range(len(self.nodes)-1):
            id0 = i * 4 + id 
            id1 = id0 + 4
            quads.append(Quad(id0, id0+1, id1, id1+1))
            quads.append(Quad(id0+1, id0+2, id1+1, id1+2))
            quads.append(Quad(id0+2, id0+3, id1+2, id1+3))
            quads.append(Quad(id0+3, id0, id1+3, id1))
        return quads



class Morphology:

    def _add_recursive_section(self, section: Section,
                            original_sec: morphio.mut.Section, root=False):
        for i, point in enumerate(original_sec.points):
            if i or root:
                node = Node(Vec3(point), original_sec.diameters[i]*0.5)
                section.add_node(node)
                # print(i,node.position)
        for original_child in original_sec.children:
            section_child = section.new_child()
            self.sections.append(section_child)
            self._add_recursive_section(section_child, original_child)

    def _build_hierarchy(self, morpho):
        self.root_sections = []
        self.sections = []
        for sec in morpho.root_sections:
            section = Section()
            self.root_sections.append(section)
            self.sections.append(section)
            self._add_recursive_section(section, sec, True)


    def __init__(self, path):
        morpho = morphio.mut.Morphology(path)

        self._build_hierarchy(morpho)
        # self.print_sections()

        quads = []
        positions = []
        normals = []
        colors = []

        for section in self.sections:
            quads += section.quads(len(positions))
            ps, ns, cs = section.geometry()
            positions += ps
            normals += ns
            colors += cs

        print(id)

        self.mesh = Mesh([],[],quads, positions, normals, colors)

    def print_sections(self):
        for sec in self.sections:
            print("Section")
            for node in sec.nodes:
                print("\t",node.position)
   


def mesh_springs_geometry(springs):
    positions = []
    normals = []
    colors = []
    color_collision = Vec3(0.8, 0.2, 0.2)
    color_no_collision = Vec3(0.2, 0.8, 0.2)

    for spring in springs:
        node0 = spring._node0
        node1 = spring._node1
        p0 = node0.position
        r0 = node0.radius
        p1 = node1.position
        r1 = node1.radius
        t = (p1-p0).normalized()
        if t.norm() == 0:
            t = Vec3(1, 0, 0)
        axis1 = Vec3(0, 0, 1)
        axis0 = (axis1.cross(t)).normalized()
        axis1 = (t.cross(axis0)).normalized()
        positions += [axis0 * r0 + p0, axis1 * r0 + p0,
                      axis0 * -r0 + p0, axis1 * -r0 + p0, t * -r0 + p0,
                      axis0 * r1 + p1, axis1 * r1 + p1,
                      axis0 * -r1 + p1, axis1 * -r1 + p1, t * r1 + p1]
        normals += [axis0 * r0, axis1 * r0, axis0 * -r0, axis1 * -r0,
                    t * -r0,
                    axis0 * r1, axis1 * r1, axis0 * -r1, axis1 * -r1,
                    t * r1]
        if node0.collide:
            colors += [color_collision] * 5
        else:
            colors += [color_no_collision] * 5
        if node1.collide:
            colors += [color_collision] * 5
        else:
            colors += [color_no_collision] * 5
    return (positions, normals, colors)


def mesh_springs(springs):
    positions, normals, colors = mesh_springs_geometry(springs)
    quads = []
    for i in range(len(springs)):
        id0 = i * 10
        id1 = id0 + 5
        quads.append(Quad(id0+4, id0+4, id0,   id0+1))
        quads.append(Quad(id0+4, id0+4, id0+1, id0+2))
        quads.append(Quad(id0+4, id0+4, id0+2, id0+3))
        quads.append(Quad(id0+4, id0+4, id0+3, id0))
        quads.append(Quad(id1+4, id1+4, id1+1,   id1))
        quads.append(Quad(id1+4, id1+4, id1+2, id1+1))
        quads.append(Quad(id1+4, id1+4, id1+3, id1+2))
        quads.append(Quad(id1+4, id1+4, id1, id1+3))
        quads.append(Quad(id0, id0+1, id1, id1+1))
        quads.append(Quad(id0+1, id0+2, id1+1, id1+2))
        quads.append(Quad(id0+2, id0+3, id1+2, id1+3))
        quads.append(Quad(id0+3, id0, id1+3, id1))
    return Mesh([], [], quads, positions, normals, colors)


def mesh_springs_update(springs, mesh):
    positions, normals, colors = mesh_springs_geometry(springs)
    mesh.update_positions(positions)
    mesh.update_normals(normals)
    mesh.update_colors(colors)
