# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

from data.data import *
from render.app import *
import sys
import os


class MorphoRender(App):

    def add_models(self, paths):
        program = ShaderProgram(
            [("shaders/quads_tess.vert", ShaderType.VERTEX),
             ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
             ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
             ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)
        program_soma = ShaderProgram(
            [("shaders/quads_tess_const.vert", ShaderType.VERTEX),
             ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
             ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
             ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)

        total_paths = float(len(paths))
        num_paths = 0
        mesh_loaded = 0
        prev_time = time.time()
        for path in paths:
            num_paths += 1
            filename, file_extension = os.path.splitext(path)
            if file_extension == ".swc":
                morpho = load_swc(path)
                mesh = mesh_from_morpho(morpho)
                self.scene.add_model((mesh, program, self.iMat))
                mesh = soma_mesh_from_morpho(morpho)
                self.scene.add_model((mesh, program_soma, self.iMat))
                mesh_loaded += 1

            print("\rLoading meshes " +
                  str(int(num_paths/total_paths*100))+"%", end='')

        print()
        num_triangles = 0
        for model in self.scene.models:
            num_triangles += model[0].num_triangles/3
        print("Loaded " + str(mesh_loaded) + " meshes with " +
              str(num_triangles/1000.0) + "K triangles in " +
              "{:.2f}".format(time.time() - prev_time) + " seconds")
        self.scene.distance = 25.0
        self.scene.level = 10


if __name__ == "__main__":
    app = MorphoRender()
    app.set_background()
    app.add_models(sys.argv[1:])
    app.run()
