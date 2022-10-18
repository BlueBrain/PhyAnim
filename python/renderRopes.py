from renderSprings import *


def rope(pos: Vec3, length: float, radius: float, num: int, ks: float,
         kd: float):
    nodes = []
    springs = []
    id = 0
    nodes.append(Node(pos, radius, id, True))
    dir = Vec3(random.random()*2-1, -random.random(),
               random.random()*2-1).normalized() * length / (num-1)
    for i in range(1, num):
        id += 1
        nodes.append(Node(pos + dir*i, radius, id))
        springs.append(Spring(nodes[id-1], nodes[id], ks, kd))

    return SpringSet(nodes, springs)


class RenderRopes(RenderSprings):

    def add_models(self):

        inc = 0.2

        for i in range(3):
            for j in range(3):
                self.sets.append(
                    rope(Vec3(i * inc, 0, j * inc),
                         1, 0.01, 10, self.ks, self.kd))


if __name__ == "__main__":
    app = RenderRopes()
    app.set_background()
    app.ks = 1000.0
    app.ksc = 1000000.0
    app.kd = 0.01
    app.dt = 0.005
    app.scene.distance = 100
    app.scene.level = 20
    app.add_models()
    app.generate_meshes()
    app.pause = True
    app.stop_on_collision = True
    app.run()
