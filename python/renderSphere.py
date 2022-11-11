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
        self.scene.add_model((self.mesh, program, mat4()))
        self.scene.distance = 50.0
        self.scene.level = 60
        self.scene.update_models()
        self.scene.radius = 12

    def update(self):
        self.t += 0.0005
        
        radius = 1

        t = abs(sin(self.t))

        match int(self.t / pi()) % 3:
            case 0:
                pos = vec3(10*t, 0, 0)
                color = vec3(t*0.5+0.5, 0, 0)
            case 1:
                pos = vec3(0, 10*t, 0)
                color = vec3(0, t*0.5+0.5, 0)
            case 2:
                pos = vec3(0, 0, 10*t)
                color = vec3(0, 0, t*0.5+0.5)

        m = translate(mat4(), pos)
        model = self.scene.models[0]
        self.scene.models[0] = (model[0], model[1], m)
        
        sphere = Sphere(vec3(), radius)

        geometry = sphere.get_geometry(color)

        self.mesh.update_positions(geometry[0])
        self.mesh.update_normals(geometry[1])
        self.mesh.update_colors(geometry[2])


if __name__ == "__main__":
    app = RenderSphere()
    # app.set_background()
    app.add_models()
    app.run()
