# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import argparse
import os 
import multiprocessing
from random import randint, random
import threading
from time import sleep
from data.data import *
from render.app import *
from anim.geometry import *
from multiprocessing import Pool, Process

def load_neuron(neuron_info):
      morpho = get_morpho(neuron_info[0])
      model = neuron_info[1]
      color = neuron_info[2]
      program = neuron_info[3]
      morpho = Morphology(morpho, model, color*0.7)
      mesh = None
      if program.primitives == GL_LINES:
          mesh = morpho.generate_lines()
      elif program.primitives == GL_PATCHES:
          mesh = morpho.generate_mesh()
      return mesh

class MorphoRenderIndex(App):

    def __init__(self):
        App.__init__(self)
        self.min = vec3()
        self.max = vec3(30)


    
    def add_models(self, sonata_file, population_name, lines=False, num = 10):
        program = ShaderProgram(
            [("shaders/lines.vert", ShaderType.VERTEX),
             ("shaders/lines.frag", ShaderType.FRAGMENT)], GL_LINES)
        self.scene.add_model((CubeMesh(self.min, self.max), program, mat4()))


        if not lines:
            program = ShaderProgram(
                [("shaders/quads_tess.vert", ShaderType.VERTEX),
                ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
                ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
                ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)

        gids = range(0,num)
        
        morpho_models = get_morpho_model(sonata_file, population_name, gids)
        
        neurons = []
        for i,info in enumerate(morpho_models):
            neurons.append((info[0], info[1], get_color(i), program))

        prev_time = time.time()   
        if len(neurons) > 0:
            self.num_models = len(neurons)
            with multiprocessing.Pool(multiprocessing.cpu_count() - 2) as pool:
                for mesh in pool.imap(load_neuron, neurons):
                    if mesh:
                        self.add_model((mesh, program, mat4()))

        self.message = ""
        num_lines = 0
        num_triangles = 0
        for model in self.scene.models:
            num_lines += model[0].num_lines/2
            num_triangles += model[0].num_triangles/3
            num_triangles += model[0].num_quads/2
        print("\rLoaded " + str(self.loaded_models) + "/" + str(self.num_models) + 
            " meshes with " + str(num_lines/1000.0) + "K lines and " +
            str(num_triangles/1000.0) + "K triangles in " +
            "{:.2f}".format(time.time() - prev_time) + " s" )

        

    
if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument("sonata_file", help="Sonata file")
    parser.add_argument("population_name", help="Population name")
  
    parser.add_argument('-lines', action='store_true', help="generate morphology meshes as lines")
    parser.add_argument('-n', type=int, default=10)
    parser.add_argument('-min_x', type=float, default=0)
    parser.add_argument('-min_y', type=float, default=0)
    parser.add_argument('-min_z', type=float, default=0)
    parser.add_argument('-max_x', type=float, default=30)
    parser.add_argument('-max_y', type=float, default=30)
    parser.add_argument('-max_z', type=float, default=30)
    args = parser.parse_args()

    app = MorphoRenderIndex()

    app.min = vec3(args.min_x, args.min_y, args.min_z)
    app.max = vec3(args.max_x, args.max_y, args.max_z)
    app.set_background()
    p = threading.Thread(target=app.add_models, args=(args.sonata_file, args.population_name, args.lines,args.n))
    p.start()
    app.scene.distance = 500.0
    app.scene.level = 5
    app.run()
