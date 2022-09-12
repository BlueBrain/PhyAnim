import random
from render.app import *
from anim.massSpring import *


class Rope:

    def __init__(
            self, pos: Vec3, length: float, radius: float, num: int, ks: float,
            kd: float):
        self._nodes = []
        self._springs = []
        self._ks = ks
        self._kd = kd
        id = 0
        self._nodes.append(Node(pos, radius, id, True))
        dir = Vec3(random.random()*2-1, -random.random(),
                   random.random()*2-1).normalized() * length / (num-1)
        # dir = Vec3(0, -1, 0).normalized() * length / (num-1)
        for i in range(1, num):
            id += 1
            self._nodes.append(Node(pos + dir*i, radius, id))
            self._springs.append(
                Spring(self._nodes[id-1], self._nodes[id], self._ks, self._kd))

        self.section = Section(self._nodes)

    def collide(self, other: 'Rope'):
        for spring in self._springs:
            for s in other._springs:
                spring.collide(s)

    def update(self, dt: float = 0.001):
        anim(self._nodes, self._springs, dt)
        self.section.update()


class RenderSprings(App):

    def __init__(self):
        App.__init__(self)
        self.ks = 10000.0
        self.kd = 10.0
        self.dt = 0.001

    def add_models(self):
        program = ShaderProgram(
            [("shaders/quads_tess.vert", ShaderType.VERTEX),
             ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
             ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
             ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)
        self.ropes = []

        for i in range(2):
            for j in range(2):
                self.ropes.append(
                    Rope(Vec3(i * 0.1, 0, j * 0.1),
                         1, 0.01, 10, self.ks, self.kd))

        for rope in self.ropes:
            self.scene.add_model((rope.section.mesh, program, self.iMat))

    def update(self):
        if not self.pause:
            for rope in self.ropes:
                rope.section.clearCollide()
            for i in range(len(self.ropes)):
                for j in range(i+1, len(self.ropes)):
                    self.ropes[i].collide(self.ropes[j])

            for rope in self.ropes:
                rope.update(self.dt)


if __name__ == "__main__":
    app = RenderSprings()
    app.set_background()
    app.ks = 100.0
    app.kd = 0.01
    app.dt = 0.01
    app.scene.distance = 100
    app.scene.level = 20
    app.add_models()
    app.run()
