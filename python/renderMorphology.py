# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import argparse
import multiprocessing
from data.data import *
from render.app import *
from anim.geometry import *
import sys
import os

def generate_model(program, path, color):
    try:
        morpho = Morphology(get_morpho(path), mat4(), color)
        if program.primitives == GL_LINES:
            mesh = morpho.generate_lines()
        elif program.primitives == GL_PATCHES:
            mesh = morpho.generate_mesh()
        if mesh:
            mat_i = mat4()
            return (mesh, program, mat_i)
        return None
    except morphio._morphio.UnknownFileType:
        return None

class MorphoRender(App):

    def __init__(self):
        App.__init__(self)

    def add_models(self, paths, lines = False):
        if not lines:
            program = ShaderProgram(
                [("shaders/quads_tess.vert", ShaderType.VERTEX),
                ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
                ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
                ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)
        else:
            program = ShaderProgram(
                [("shaders/lines.vert", ShaderType.VERTEX),
                ("shaders/lines.frag", ShaderType.FRAGMENT)], GL_LINES)

        self.num_models = float(len(paths))        
        prev_time = time.time()

        with multiprocessing.Pool(multiprocessing.cpu_count() - 2) as pool:
            for i,path in enumerate(paths):
                pool.apply_async(
                        generate_model, args=(program, path, self.get_color(i)), callback=self.add_model)
            pool.close()
            pool.join()
        # for i,path in enumerate(paths):
        #     self.add_model(generate_model(program, path, self.get_color(i)))
        self.message = ""
        
        num_lines = 0
        num_triangles = 0
        for model in self.scene.models:
            num_lines += model[0].num_lines/2
            num_triangles += model[0].num_triangles/3
            num_triangles += model[0].num_quads/2
        print("\rLoaded " + str(self.loaded_models) + "/" + str(len(paths)) + 
            " meshes with " + str(num_lines/1000.0) + "K lines and " +
            str(num_triangles/1000.0) + "K triangles in " +
            "{:.2f}".format(time.time() - prev_time) + " seconds." )

if __name__ == "__main__":

    parser = argparse.ArgumentParser() 
    parser.add_argument('--lines', action='store_true', help="generate morphology meshes as lines")
    parser.add_argument('paths', metavar='path', nargs='+',
                        help='path to neuron moprholgy file')
    args = parser.parse_args()    
    
    app = MorphoRender()
    app.set_background()
    p = threading.Thread(target=app.add_models, args=(args.paths,args.lines))
    p.start()
    # app.add_models(args.paths, args.lines)
    app.scene.level = 5
    app.scene.distance = 500
    app.run()
