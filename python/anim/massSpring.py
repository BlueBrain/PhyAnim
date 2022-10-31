import multiprocessing
from anim.geometry import *


def anim(nodes, springs, dt: float, gravity: bool = True, dynamic: bool = True):
    if (gravity):
        for node in nodes:
            node.force += Vec3(0, -9.8, 0)*node.mass
    for spring in springs:
        spring.add_forces()
    for node in nodes:
        node.update(dt, dynamic)


def collide_spring(spring0, spring1, ksc):
    p0, a0, p1, a1 = project_segment_segment(
        spring0._node0.position, spring0._node1.position,
        spring1._node0.position, spring1._node1.position)
    r0 = min(spring0._node0.radius, spring0._node1.radius)
    r1 = min(spring1._node0.radius, spring1._node1.radius)
    dir = p1 - p0
    x = dir.norm() - (r0 + r1)
    if x < 0.0:
        dir.normalized()
        f = dir*ksc*x
        spring0._node0.force += f * (1-a0)
        spring0._node1.force += f * a0
        spring1._node0.force -= f * (1-a1)
        spring1._node1.force -= f * a1
        spring0._node0.collide = True
        spring0._node1.collide = True
        spring1._node0.collide = True
        spring1._node1.collide = True
        return 1
    return 0


class SpringSet:

    def __init__(self, nodes, springs):
        self.nodes = nodes
        self.springs = springs
        self.aabb = AABoundingBox()
        # for spring in self.springs:
        self.aabb.elements += self.springs
        self.aabb.divide()
        self.aabb.update()

    def clear(self):
        for node in self.nodes:
            node.clear()

    def anim(self, dt: float, gravity: bool = True, dynamic: bool = True):
        anim(self.nodes, self.springs, dt, gravity, dynamic)
        self.aabb.update()


def collide_spring_set(set0, set1, ksc: float):
    collisions = []
    pairs = set0.aabb.colliding_pairs(set1.aabb)
    for pair in pairs:
        if collide_spring(pair[0], pair[1], ksc):
            collisions.append(pair)
    return collisions


def collide_spring_sets(sets, ksc: float):
    n = len(sets)
    collisions = []
    for i in range(n-1):
        for j in range(i+1, n):
            collisions += collide_spring_set(sets[i], sets[j], ksc)
    return collisions


def clear_sets(sets):
    for set in sets:
        set.clear()


def anim_sets(sets, dt, gravity: bool = True, dynamic: bool = True):
    for set in sets:
        set.anim(dt, gravity, dynamic)


def update_sets(sets, meshes, color, color_collision):
    for i, set in enumerate(sets):
        mesh = meshes[i]
        mesh_springs_update(set.springs, mesh, color, color_collision)


def morphology_to_spring_set(morpho: Morphology):
    springs = []
    for section in morpho.sections:
        for i in range(len(section.nodes)-1):
            springs.append(morpho.nodes[i], morpho.nodes[i+1])
    return SpringSet(morpho.nodes, springs)
