# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import argparse
import multiprocessing
import random
from anim.massSpring import SpringSet, anim_sets, clear_sets, collide_spring_sets
from data.data import *
from render.app import *
from anim.geometry import *
import sys
import os

class Overlap(App):

    def __init__(self):
        App.__init__(self)
        self.morphologies = []
        self.spring_sets = []

        self.ks = 2000.0
        self.ksc = 200.0
        self.kd = 0.0
        self.dt = 0.001
        self.col = -1
        self.colSpring = 0
        self.pause = True
        self.ksc_factor = 1.02

    def key_callback(self, window, key, scancode, action, mods):
        App.key_callback(self, window, key, scancode, action, mods)
     
        if action == glfw.PRESS:
            if key == glfw.KEY_SPACE:
                self.pause = not self.pause
                if self.pause:
                    self.message = "Pause"
                else:
                    self.message = "Resume"
                    self.start_time = time.time()
            if key == glfw.KEY_R:
                n = len(self.collisions)
                if n:
                    self.col += 1
                    self.col %= n
                    self.colSpring = 0
                    self.message = "\tCollision " + str(self.col+1) + "/" + str(n)
                    col = self.collisions[self.col]
                    self.scene.target = col[self.colSpring].center()
                    self.scene.radius = col[self.colSpring].length() * 3
            elif key == glfw.KEY_F:
                n = len(self.collisions)
                if n:
                    self.col -= 1
                    self.col %= n
                    self.colSpring = 0
                    self.message = "\tCollision " + str(self.col+1) + "/" + str(n)
                    col = self.collisions[self.col]
                    self.scene.target = col[self.colSpring].center()
                    self.scene.radius = col[self.colSpring].length() * 3
            elif key == glfw.KEY_T:
                n = len(self.collisions)
                if n:
                    self.colSpring += 1
                    self.colSpring %= 2
                    self.message = "\tCollision " + str(self.col+1) + "/" + str(n)
                    col = self.collisions[self.col]
                    self.scene.target = col[self.colSpring].center()
                    self.scene.radius = col[self.colSpring].length() * 3
            elif key == glfw.KEY_G:
                n = len(self.collisions)
                if n:
                    self.colSpring -= 1
                    self.colSpring %= 2
                    self.message = "\tCollision " + str(self.col+1) + "/" + str(n)
                    col = self.collisions[self.col]
                    self.scene.target = col[self.colSpring].center()
                    self.scene.radius = col[self.colSpring].length() * 3

    def add_morphology(self, path, program):
        try:
            color = self.get_color(self.loaded_models)
            morpho = Morphology(path,color*0.5,color)
            if self.n_morpho:
                id0 = 478
                id1 = id0 + 1
                morpho.sections = morpho.sections[id0:id1]
            else:
                id0 = 136
                id1 = id0 + 1
                morpho.sections = morpho.sections[id0:id1]

            nodes = {}
            for sec in morpho.sections:
                for node in sec.nodes:
                    nodes[node] = node
            morpho.nodes = nodes.values()


            if morpho:
                if program.primitives == GL_LINES:
                    morpho.generate_lines()
                elif program.primitives == GL_PATCHES:
                    morpho.generate_mesh()
                self.spring_sets.append(SpringSet(morpho.nodes, morpho.get_springs(self.ks, self.kd)))
                self.morphologies.append(morpho)

                mat_i = Mat4()
                mat_i.identity()
                self.scene.add_model((morpho.mesh, program, mat_i))
                self.loaded_models += 1
                self.message = "Loading models " + str(
                    int(self.loaded_models/self.num_models*100))+"%" 
        except morphio._morphio.UnknownFileType:
            pass
      

    def add_models(self, paths, lines = False):
        self.quads = not lines
        if self.quads:
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

        # for path in paths:
        #     self.add_morphology(path, program)

        self.n_morpho = 0
        self.add_morphology("/home/jjgarcia/data/swc/layers/L1_DAC.swc", program)
        self.n_morpho = 1
        self.add_morphology("/home/jjgarcia/data/swc/layers/L1_LAC.swc", program)



        clear_sets(self.spring_sets)
        self.collisions = collide_spring_sets(self.spring_sets, self.ksc)
        self.col = -1
        self.l = 0.0
        for morpho in self.morphologies:
            for section in morpho.sections:
                for i in range(len(section.nodes)-1):
                    self.l += distance(section.nodes[i].position, section.nodes[i+1].position)
            if self.quads:
                morpho.update_mesh()
            else:
                morpho.update_lines()
            
        self.message = "Number of collisions: " + str(len(self.collisions))
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
    
    def update(self):
        if not self.pause:
            prev_total = time.time()
            clear_sets(self.spring_sets)
            n = len(collide_spring_sets(self.spring_sets, self.ksc))
            if n:
                self.ksc *= self.ksc_factor
                anim_sets(self.spring_sets, self.dt, False, False)
                for col in self.collisions:
                    col[0]._node0.collide = True
                    col[0]._node1.collide = True
                    col[1]._node0.collide = True
                    col[1]._node1.collide = True
                for morpho in self.morphologies:
                    if self.quads:
                        morpho.update_mesh()
                    else:
                        morpho.update_lines()

                
                self.message = "Number of collisions: " + str(
                    n) + " Update: " + "{:.4f}".format(time.time() - prev_total) + " sg"
            else:
                self.pause = True
                for col in self.collisions:
                    col[0]._node0.collide = True
                    col[0]._node1.collide = True
                    col[1]._node0.collide = True
                    col[1]._node1.collide = True
                l = 0.0
                for morpho in self.morphologies:
                    for section in morpho.sections:
                        for i in range(len(section.nodes)-1):
                            l += distance(section.nodes[i].position, section.nodes[i+1].position)
                    if self.quads:
                        morpho.update_mesh()
                    else:
                        morpho.update_lines()
                print(LINE_CLEAR,end="")
                print("\r----Collisions solved in " +
                    "{:.2f}".format(time.time() - self.start_time) + " sg. Length " + 
                    "{:.2f}".format(l) + "/" + "{:.2f}".format(self.l) + " - diff: " +
                    "{:.2f}".format(abs(self.l - l) / self.l * 100) + "%")
                    
                
if __name__ == "__main__":

    parser = argparse.ArgumentParser() 
    parser.add_argument('--lines', action='store_true', help="generate morphology meshes as lines")
    parser.add_argument('paths', metavar='path', nargs='+',
                        help='path to neuron moprholgy file')
    args = parser.parse_args()    
    
    app = Overlap()
    app.set_background()
    app.ks = 2000.0
    app.ksc = 200.0
    app.ksc_factor = 1.02
    app.kd = 0
    app.dt = 0.001
    app.add_models(args.paths, args.lines)
    app.scene.level = 5
    app.scene.distance = 500
    app.run()
