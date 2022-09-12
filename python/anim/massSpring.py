from anim.geometry import *


def anim(nodes, springs, dt: float = 0.001):
    for node in nodes:
        node.force = Vec3(0, -9.8, 0)*node.mass

    for spring in springs:
        spring.add_forces()

    for node in nodes:
        node.update(dt)

        if node.position.y < -2:
            node.position.y = -2
