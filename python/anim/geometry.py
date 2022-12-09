from __future__ import annotations
from calendar import c
from mailbox import NoSuchMailboxError
from platform import node

from render.mesh import *
import morphio.mut


class Node:

    def __init__(self, position: vec3, radius: float = 1.0, id: int = 0,
                 fixed: bool = False, mass: float = 1.0, velocity: vec3 = vec3(),
                 force: vec3 = vec3()):
        self.position = position
        self.radius = radius
        self.id = id
        self.fixed = fixed
        self.mass = mass
        self.velocity = velocity
        self.force = force
        self.collide = False

    def update(self, dt: float, dynamic: bool = True):
        if not self.fixed:
            a = self.force / self.mass
            v = self.velocity + a*dt
            self.position += v * dt
            if dynamic:
                self.velocity = v

    def clear(self):
        self.collide = False
        self.force = vec3()

    def __str__(self):
        return "nodes.append(Node(vec3" + str(self.position) + ", " + str(self.radius) + "))"


class Spring:

    def __init__(
            self, node0: Node, node1: Node, ks: float = 1000.0, kd: float = 0.1):
        self._node0 = node0
        self._node1 = node1
        self._ks = ks
        self._kd = kd
        self._lenght = length(node1.position - node0.position)

    def __str__(self):
        return "\t" + str(self._node0) + " \n\t" + str(self._node1)

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
        return min(self._node0.position - vec3(self._node0.radius),
            self._node1.position - vec3(self._node1.radius))

    def max(self):
        return max(self._node0.position + vec3(self._node0.radius),
            self._node1.position + vec3(self._node1.radius))

    def aabb(self):
        aabb = AABoundingBox()
        aabb.add_pos(self.min())
        aabb.add_pos(self.max())
        return aabb

    def center(self):
        return (self._node0.position + self._node1.position)*0.5

    def length(self):
        return distance(self._node0.position, self._node1.position)

    def add_forces(self):
        r = self._lenght
        if r > 0.0:
            pd = self._node1.position - self._node0.position
            l = length(pd)
            vd = self._node1.velocity - self._node0.velocity
            force = vec3()
            if l > 0.0:
                force = pd * (self._ks * (l / r - 1.0) +
                              self._kd * (dot(vd,pd) / (l * r))) / l
                self._node0.force += force
                self._node1.force -= force


class Section:
    def __init__(self):
        self.children = []
        self.nodes = []
        self.aabb = AABoundingBox()

    def add_node(self, node: Node):
        self.nodes.append(node)
        self.aabb.add_pos(node.position)

    def new_child(self):
        section = Section()
        self.children.append(section)
        section.add_node(self.nodes[-1])
        return section

    def oriented_axis(self, t: vec3):
        if length(t) == 0:
            t = vec3(1, 0, 0)
        z = normalize(t)
        y = vec3(0, 0, 1)
        x = normalize(cross(y,z))
        y = normalize(cross(z,x))
        return (x, y, z)

    def geometry(self, color_no_collision, color_collision):
        positions = []
        normals = []
        colors = []
        for i, node in enumerate(self.nodes):
            p = node.position
            r = node.radius
            t = vec3(1, 0, 0)
            if i == 0:
                node0 = node
                node1 = self.nodes[i+1]
            elif i == len(self.nodes)-1:
                node0 = self.nodes[i-1]
                node1 = node
            else:
                node0 = self.nodes[i-1]
                node1 = self.nodes[i+1]
            t = node1.position - node0.position
            x, y, z = self.oriented_axis(t)
            positions += [x*r+p, y*r+p, x*-r+p, y*-r+p]
            normals += [x*r, y*r, x*-r, y*-r]
            if node.collide:
                colors += [color_collision] * 4
            else:
                colors += [color_no_collision] * 4

        p0 = self.nodes[0].position
        r0 = self.nodes[0].radius
        t0 = (self.nodes[1].position - p0)
        x, y, z = self.oriented_axis(t0)
        positions.append(z*-r0+p0)
        normals.append(z*-r0)
        if self.nodes[0].collide:
            colors.append(color_collision)
        else:
            colors.append(color_no_collision)
        pi = self.nodes[-1].position
        ri = self.nodes[-1].radius
        ti = (self.nodes[-2].position - pi)
        x, y, z = self.oriented_axis(ti)
        positions.append(z*-ri+pi)
        normals.append(z*-ri)
        if self.nodes[-1].collide:
            colors.append(color_collision)
        else:
            colors.append(color_no_collision)

        return (positions, normals, colors)

    def quads(self, id=0):
        quads = []
        for i in range(len(self.nodes)-1):
            id0 = i * 4 + id
            id1 = id0 + 4
            quads.append(Quad(id0, id0+1, id1, id1+1))
            quads.append(Quad(id0+1, id0+2, id1+1, id1+2))
            quads.append(Quad(id0+2, id0+3, id1+2, id1+3))
            quads.append(Quad(id0+3, id0, id1+3, id1))

        id0 = id
        id1 = id + len(self.nodes)*4
        quads.append(Quad(id1, id1, id0, id0+1))
        quads.append(Quad(id1, id1, id0+1, id0+2))
        quads.append(Quad(id1, id1, id0+2, id0+3))
        quads.append(Quad(id1, id1, id0+3, id0))

        id0 = id + (len(self.nodes) - 1) * 4
        id1 = id + len(self.nodes)*4 + 1
        quads.append(Quad(id1, id1, id0, id0+3))
        quads.append(Quad(id1, id1, id0+3, id0+2))
        quads.append(Quad(id1, id1, id0+2, id0+1))
        quads.append(Quad(id1, id1, id0+1, id0))
        return quads

    def geometry_lines(self):
        positions = []
        normals = []
        colors = []
        for node in self.nodes:
            positions.append(node.position)
            normals.append(vec3(0, 0, 1))
            colors.append(vec3(0.4, 0.8, 0.4))
        return (positions, normals, colors)

    def lines(self, id=0):
        lines = []
        for i in range(len(self.nodes)-1):
            lines.append(Line(id+i, id+i+1))
        return lines


class Morphology:

    def __init__(self, morphology, model=mat4(), color=vec3(0, 1, 0), color_collide=vec3(1, 0, 0)):
        self.model = model
        self._create_hierarchy(morphology)
        self.color_collide = color_collide
        self.color = color
        
    def _add_recursive_section(self, section: Section,
                               original_sec: morphio.mut.Section, root=False):
        for i, point in enumerate(original_sec.points):
            if i or root:
                radius = original_sec.diameters[i]*0.5
                node = Node(self.model*vec3(point), radius, 0, False, radius)
                section.add_node(node)
        for original_child in original_sec.children:
            section_child = section.new_child()
            self.sections.append(section_child)
            self._add_recursive_section(section_child, original_child)

    def _create_hierarchy(self, morpho):
        self.root_sections = []
        self.sections = []
        self.soma_nodes = []
        self.soma_center = vec3()
        self.soma_radius = 0
        for i, p in enumerate(morpho.soma.points):
            point = self.model*vec3(p)
            node = Node(point, morpho.soma.diameters[i] * 0.5)
            self.soma_nodes.append(node)
            self.soma_center += point
        self.soma_center /= len(morpho.soma.points)
        for node in self.soma_nodes:
            self.soma_radius = max(self.soma_radius, distance(
                self.soma_center, node.position))
        for sec in morpho.root_sections:
            section = Section()
            self.root_sections.append(section)
            self.sections.append(section)
            self._add_recursive_section(section, sec, True)

        unique_nodes = {}
        for section in self.sections:
            for node in section.nodes:
                unique_nodes[node] = node
        self.nodes = unique_nodes.values()
        for i, node in enumerate(self.nodes):
            node.id = i

    def cut(self, aabb):
        soma_aabb = AABoundingBox()
        soma_aabb.add_pos(self.soma_center + self.soma_radius)
        soma_aabb.add_pos(self.soma_center - self.soma_radius)

        if not soma_aabb.is_colliding(aabb):
            self.soma_nodes = None
        
        sections = []
        for section in self.sections:
            if section.aabb.is_colliding(aabb):
                sections.append(section)
        self.sections = sections

    def generate_mesh(self):
        quads = []
        positions = []
        normals = []
        colors = []

        if self.soma_nodes:
            sphere = Sphere(self.soma_center, self.soma_radius)
            quads += sphere.get_quads(0)
            pos, nor, col = sphere.get_geometry(self.color)
            positions += pos
            normals += nor
            colors += col

        for section in self.sections:
            quads += section.quads(len(positions))
            pos, nor, col = section.geometry(self.color, self.color_collide)
            positions += pos
            normals += nor
            colors += col

        mesh = Mesh([], [], quads, positions, normals, colors)

        return mesh

    def generate_lines(self, aabb = None):
        lines = []
        positions = []
        colors = []

        for node in self.nodes:
            positions.append(node.position)
            colors.append(self.color)

        for section in self.sections:
            for i in range(len(section.nodes)-1):
                node0 = section.nodes[i]
                node1 = section.nodes[i+1]
                lines.append(Line(node0.id, node1.id))

        mesh = Mesh(lines, [], [], positions, [], colors)
        return mesh

    def update_mesh(self, mesh, aabb = None):
        positions = []
        normals = []
        colors = []

        if self.soma_nodes:
            sphere = Sphere(self.soma_center, self.soma_radius)
            pos, nor, col = sphere.get_geometry(self.color)
            positions += pos
            normals += nor
            colors += col

        for section in self.sections:
            pos, nor, col = section.geometry(self.color, self.color_collide)
            positions += pos
            normals += nor
            colors += col
        mesh.update_positions(positions)
        mesh.update_normals(normals)
        mesh.update_colors(colors)

    def update_lines(self, mesh):
        positions = []
        colors = []

        for node in self.nodes:
            positions.append(node.position)
            if node.collide:
                colors.append(self.color_collide)
            else:
                colors.append(self.color)
        mesh.update_positions(positions)
        mesh.update_colors(colors)

    def print_sections(self):
        for sec in self.sections:
            print("Section")
            for node in sec.nodes:
                print("\t", node.position)

    def get_springs(self, ks, kd):
        springs = []
        for section in self.sections:
            for i in range(len(section.nodes)-1):
                springs.append(
                    Spring(section.nodes[i], section.nodes[i+1], ks, kd))
        return springs


def collide(morpho: Morphology, aabb: AABoundingBox):
    for section in morpho.sections:
        for node in section.nodes:
            if aabb.is_inside(node.position):
                return True
    return False

def mesh_springs_geometry(springs, color, color_collision):
    positions = []
    normals = []
    colors = []
    for spring in springs:
        node0 = spring._node0
        node1 = spring._node1
        p0 = node0.position
        r0 = node0.radius
        p1 = node1.position
        r1 = node1.radius
        t = normalize(p1-p0)
        if length(t) == 0:
            t = vec3(1, 0, 0)
        axis1 = vec3(0, 0, 1)
        axis0 = normalize(cross(axis1,t))
        axis1 = normalize(cross(t,axis0))
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
            colors += [color] * 5
        if node1.collide:
            colors += [color_collision] * 5
        else:
            colors += [color] * 5
    return (positions, normals, colors)


def mesh_springs(springs, color, color_collision):
    positions, normals, colors = mesh_springs_geometry(
        springs, color, color_collision)
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


def mesh_springs_update(springs, mesh, color, color_collision):
    positions, normals, colors = mesh_springs_geometry(
        springs, color, color_collision)
    mesh.update_positions(positions)
    mesh.update_normals(normals)
    mesh.update_colors(colors)
