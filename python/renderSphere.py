from render.app import *


class RenderSphere(App):

    def add_models(self):
        self.t = 0.0

        program = ShaderProgram(
            [("shaders/quads_tess_const.vert", ShaderType.VERTEX),
             ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
             ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
             ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)
        self.mesh = SphereMesh()
        self.scene.add_model((self.mesh, program, self.iMat))
        self.scene.distance = 50.0
        self.scene.level = 60

    def update(self):
        self.t += 0.001
        color = Vec3(math.cos(self.t)*0.4 + 0.6,
                     math.cos(self.t + math.pi*0.5)*0.4 + 0.6, 0)
        radius = math.cos(self.t)*0.4 + 0.6

        sphere = Sphere(Vec3(), radius)
        geometry = sphere.get_geometry(color)

        self.mesh.update_positions(geometry[0])
        self.mesh.update_normals(geometry[1])
        self.mesh.update_colors(geometry[2])


if __name__ == "__main__":
    app = RenderSphere()
    app.set_background()
    app.add_models()
    app.run()
