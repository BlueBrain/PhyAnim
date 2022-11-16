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
from multiprocessing import Pool, Process


def joint_aabbs(aabbs):
    jointed = True
    while(jointed):
        jointed = False
        for i, aabb in enumerate(aabbs):
            for other in aabbs[i+1:]:
                if aabb.is_colliding(other):
                    aabb.add_aabb(other)
                    aabbs.remove(other)
                    jointed = True


def collisions_2_aabb(cols):
    aabbs = []
    for col in cols:
        aabb = AABoundingBox()
        aabb.add_aabb(col[0].aabb())
        aabb.add_aabb(col[1].aabb())
        aabbs.append(aabb)
    joint_aabbs(aabbs)
    return aabbs


class Overlap(App):

    def __init__(self):
        App.__init__(self)
        self.morphologies = []
        self.morpho_meshes = []
        self.spring_sets = []
        self.cur_spring_sets = []
        self.meshes = []

        self.ks = 2000.0
        self.ksc = 200.0
        self.cur_ksc = 0
        self.kd = 0.0
        self.dt = 0.001
        self.col = -1
        self.ksc_factor = 1.02

        self.pause = True
        self.stop_on_solve = True
        self.solved = False
        self.solving = False

    def setup_collision(self, aabb):
        self.cur_ksc = self.ksc
        self.colors = []
        self.l = 0
        self.start_time = time.time()
        self.cur_spring_sets = []
        self.meshes = []
        del self.scene.models[1:]
        for i, set in enumerate(self.spring_sets):

            springs = []
            nodes = []
            for spring in set.springs:
                if aabb.is_colliding(spring.aabb()):
                    springs.append(spring)
                    self.l += spring.length()
                    node0 = spring._node0
                    node1 = spring._node1
                    node0.fixed = not aabb.is_inside(node0.position)
                    node1.fixed = not aabb.is_inside(node1.position)
                    nodes += [spring._node0, spring._node1]
            nodes = list(dict.fromkeys(nodes))
            if len(springs) > 0:
                color = self.get_color(i)
                self.colors.append(color)
                self.cur_spring_sets.append(
                    SpringSet(nodes, springs))
                mesh = mesh_springs(springs, color*0.5, color)
                self.meshes.append(mesh)
                self.scene.add_model(
                    (mesh, self.program, self.iMat))
        self.scene.update_models()
        self.scene.target = aabb.center()
        self.scene.radius = aabb.radius() * 2

    def key_callback(self, window, key, scancode, action, mods):
        App.key_callback(self, window, key, scancode, action, mods)
        if action == glfw.PRESS:
            if key == glfw.KEY_SPACE:
                self.pause = not self.pause
                if self.solved:
                    self.col += 1
                    self.col %= len(self.collisions)
                    self.scene.target = self.collisions[self.col].center()
                    self.scene.radius = self.collisions[self.col].radius() * 2

    def add_morphology(self, path):
        try:
            color = self.get_color(self.loaded_models)
            morpho = Morphology(path, color*0.5, color)

            if morpho:
                self.morphologies.append(morpho)
                self.loaded_models += 1
                self.message = "Loading models " + str(
                    int(self.loaded_models/self.num_models*100))+"%"
        except morphio._morphio.UnknownFileType:
            pass

    def add_models(self, paths, lines=False):
        self.quads = not lines
        if self.quads:
            self.program = ShaderProgram(
                [("shaders/quads_tess.vert", ShaderType.VERTEX),
                 ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
                 ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
                 ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)
        else:
            self.program = ShaderProgram(
                [("shaders/lines.vert", ShaderType.VERTEX),
                 ("shaders/lines.frag", ShaderType.FRAGMENT)], GL_LINES)

        self.num_models = float(len(paths))
        prev_time = time.time()

        for path in paths:
            self.add_morphology(path)

        self.total_lenght = 0.0
        for morpho in self.morphologies:
            set = SpringSet(morpho.nodes, morpho.get_springs(self.ks, self.kd))
            self.spring_sets.append(set)
            for spring in set.springs:
                self.total_lenght += spring.length()

        self.collisions = collisions_2_aabb(
            collide_spring_sets(self.spring_sets, self.ksc))

        for morpho in self.morphologies:
            if self.program.primitives == GL_LINES:
                mesh = morpho.generate_lines()
            elif self.program.primitives == GL_PATCHES:
                mesh = morpho.generate_mesh()
            self.morpho_meshes.append(mesh)

        for mesh in self.morpho_meshes:
            self.scene.add_model((mesh, self.program, mat4()))

        self.message = "Number of collisions: " + str(len(self.collisions))
        self.total_start_time = time.time()
        num_lines = 0
        num_triangles = 0
        for model in self.scene.models:
            num_lines += model[0].num_lines/2
            num_triangles += model[0].num_triangles/3
            num_triangles += model[0].num_quads/2
        print("Loaded " + str(self.loaded_models) + "/" + str(len(paths)) +
              " meshes with " + str(num_lines/1000.0) + "K lines and " +
              str(num_triangles/1000.0) + "K triangles in " +
              "{:.2f}".format(time.time() - prev_time) + " seconds.")

    def update(self):
        if not self.pause:
            if not self.solved:
                if self.solving:
                    clear_sets(self.cur_spring_sets)
                    n = len(collide_spring_sets(
                        self.cur_spring_sets, self.cur_ksc))
                    if n:
                        self.cur_ksc *= self.ksc_factor
                        anim_sets(self.cur_spring_sets, self.dt, False, False)
                        clear_sets(self.cur_spring_sets)
                        collide_spring_sets(self.cur_spring_sets, self.cur_ksc)
                        for i, mesh in enumerate(self.meshes):
                            color = self.colors[i]
                            mesh_springs_update(
                                self.cur_spring_sets[i].springs, mesh, color*0.5, color)
                    else:
                        l = 0.0
                        for set in self.cur_spring_sets:
                            for spring in set.springs:
                                l += spring.length()
                        
                        if self.verbose:
                            print(end=LINE_CLEAR)
                            print("\r", end="")
                        print("- Collisions {:d}/{:d} solved in {:.04f} sc. Length {:.2f}/{:.2f} - diff: {:.2f}%".format(
                            self.col + 1, len(self.collisions), 
                            time.time() - self.start_time, l, self.l, 
                            abs(self.l - l)/self.l * 100))
                        self.solving = False
                        if self.stop_on_solve:
                            self.pause = True
                else:
                    self.col += 1
                    if self.col < len(self.collisions):
                        self.setup_collision(self.collisions[self.col])
                        self.solving = True
                        if self.stop_on_solve:
                            self.pause = True
                    else:
                        self.col = 0
                        self.solved = True
                        del self.scene.models[1:]
                        self.scene.aabb = AABoundingBox()

                        total_length = 0
                        for set in self.spring_sets:
                            for spring in set.springs:
                                total_length += spring.length()

                        for aabb in self.collisions:
                            for set in self.spring_sets:
                                for node in set.nodes:
                                    if aabb.is_inside(node.position):
                                        node.collide = True
                        for i, morpho in enumerate(self.morphologies):
                            if self.quads:
                                morpho.update_mesh(self.morpho_meshes[i])
                            else:
                                morpho.update_lines(self.morpho_meshes[i])
                        self.scene.aabb = AABoundingBox()
                        for mesh in self.morpho_meshes:
                            self.scene.add_model((mesh, self.program,mat4()))

                        self.message = ""

                        if self.verbose:
                            print(end=LINE_CLEAR)
                            print("\r")
                        print("-- {:d} collisions solved in {:.04f} sc. Length {:.2f}/{:.2f} - diff: {:.6f}%".format(
                            len(self.collisions),
                             time.time() - self.total_start_time, 
                             total_length, self.total_lenght, 
                             abs(self.total_lenght - total_length)/self.total_lenght * 100))



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-lines', action='store_true',
                        help="generate morphology meshes as lines")
    parser.add_argument(
        '-cont', action='store_true',
        help="solve collisions in a continuous mode")
    parser.add_argument(
        '-nfps', action='store_true', help="deactivate fps show")

    parser.add_argument('paths', metavar='path', nargs='+',
                        help='path to neuron moprholgy file')
    args = parser.parse_args()


    app = Overlap()
    app.verbose = not args.nfps
    app.set_background()
    app.ks = 2000.0
    app.ksc = 100.0
    app.ksc_factor = 1.05
    app.kd = 0
    app.dt = 0.001


    app.stop_on_solve = not args.cont

    app.add_models(args.paths, args.lines)
    app.scene.level = 5
    app.scene.distance = 500
    app.run()
