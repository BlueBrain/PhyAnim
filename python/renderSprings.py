import random
from render.app import *
from anim.massSpring import *


def line(pos: Vec3, dir: Vec3, length: float, radius: float, num: int,
         ks: float, kd: float):
    id = 0
    dir.normalized()
    dir *= length / (num-1)
    nodes = []
    springs = []
    nodes.append(Node(pos, radius, id, True))
    for i in range(1, num):
        id += 1
        fixed = i == num-1
        nodes.append(Node(pos + dir*i, radius, id, fixed))
        springs.append(Spring(nodes[id-1], nodes[id], ks, kd))
    return SpringSet(nodes, springs)


class RenderSprings(App):

    def __init__(self):
        App.__init__(self)
        self.ks = 1000.0
        self.ksc = 100.0
        self.kd = 10.0
        self.dt = 0.0001
        self.pause = False
        self.gravity = True
        self.collisions_check = True
        self.stop_on_collision = False
        self.sets = []
        self.program = ShaderProgram(
            [("shaders/quads_tess.vert", ShaderType.VERTEX),
             ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
             ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
             ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)

    def key_callback(self, window, key, scancode, action, mods):
        App.key_callback(self, window, key, scancode, action, mods)
        if action == glfw.PRESS or action == glfw.REPEAT:
            if key == glfw.KEY_SPACE:
                self.pause = not self.pause
                if self.pause:
                    self.message = "Pause"
                else:
                    self.message = "Resume"
            elif key == glfw.KEY_G:
                self.gravity = not self.gravity
                if self.gravity:
                    self.message = "Gravity on"
                else:
                    self.message = "Gravity off"
            elif key == glfw.KEY_C:
                self.collisions_check = not self.collisions_check
                if self.collisions_check:
                    self.message = "Collisions on"
                else:
                    self.message = "Collisions off"
            elif key == glfw.KEY_V:
                self.stop_on_collision = not self.stop_on_collision
                if self.stop_on_collision:
                    self.message = "Stop on collision on"
                else:
                    self.message = "Stop on collision off"
                    self.pause = False

    def add_models(self):
        self.sets.append(line(Vec3(-1, 0, 0),
                              Vec3(1, 0, 0),
                              2.0, 0.1, 10, self.ks, self.kd))
        self.sets.append(line(Vec3(0, -1.0, 0.01),
                              Vec3(0, 1.0, 0),
                              2.0, 0.1, 10, self.ks, self.kd))

    def generate_meshes(self):
        for set in self.sets:
            self.scene.add_model((set.mesh, self.program, self.iMat))

    def update(self):
        if not self.pause:
            for set in self.sets:
                set.clear()

            if self.collisions_check:
                collide_spring_sets(self.sets, self.ksc)

            for set in self.sets:
                set.anim(self.dt, self.gravity)

            if self.collisions_check and self.stop_on_collision:
                for set in self.sets:
                    set.clear()
                num = collide_spring_sets(self.sets, self.ksc)
                if (num):
                    self.message = "Number of collision: " + str(num)
                    self.pause = True

            for set in self.sets:
                set.update()


if __name__ == "__main__":
    app = RenderSprings()
    app.set_background()
    app.ks = 1000.0
    app.ksc = 10000.0
    app.kd = 10
    app.dt = 0.005
    app.scene.distance = 100
    app.scene.level = 20
    app.add_models()
    app.generate_meshes()
    app.gravity = False
    app.run()
