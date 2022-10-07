from anim.geometry import *


def anim(nodes, springs, dt: float, gravity: bool = True):

    if (gravity):
        for node in nodes:
            node.force += Vec3(0, -9.8, 0)*node.mass

    for spring in springs:
        spring.add_forces()

    for node in nodes:
        node.update(dt)

        if node.position.y < -2:
            node.position.y = -2


def collide_spring(spring0, spring1, ksc):
    p0, p1 = project_segment_segment(
        spring0._node0.position, spring0._node1.position,
        spring1._node0.position, spring1._node1.position)
    r0 = max(spring0._node0.radius, spring0._node1.radius)
    r1 = max(spring1._node0.radius, spring1._node1.radius)
    dir = p1 - p0
    x = dir.norm() - (r0 + r1)
    if x < 0.0:
        dir.normalized()
        f = dir*ksc*x
        spring0._node0.force += f
        spring0._node1.force += f
        spring1._node0.force -= f
        spring1._node1.force -= f
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

        self.mesh = mesh_springs(self.springs)
        self.aabb = AABoundingBox()
        for spring in self.springs:
            self.aabb.elements.append(spring)
        self.aabb.update()

    def clear(self):
        for node in self.nodes:
            node.clear()

    def anim(self, dt: float, gravity: bool):
        anim(self.nodes, self.springs, dt, gravity)
        self.aabb.update()

    def update(self):
        mesh_springs_update(self.springs, self.mesh)


def collide_spring_set(set0, set1, ksc: float):
    num = 0
    if set0.aabb.is_colliding(set1.aabb):
        for spring0 in set0.springs:
            for spring1 in set1.springs:
                num += collide_spring(spring0, spring1, ksc)
    return num


def collide_spring_sets(sets, ksc: float):
    n = len(sets)
    num = 0
    for i in range(n):
        for j in range(i+1, n):
            num += collide_spring_set(sets[i], sets[j], ksc)
    return num
