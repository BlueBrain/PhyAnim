# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import argparse
import multiprocessing
import random
from anim.massSpring import SpringSet, anim_sets, clear_sets, collide_spring_sets
from data.data import *
from render.app import *
from anim.geometry import *
import sys
import libsonata
import os
from multiprocessing import Pool, Process

total = 0
id = 0


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

def check_morpho(morpho_path, model, aabb):
    morpho = Morphology(morpho_path, model)
    result = -1
    if collide(morpho, aabb):
        print("+")
        return morpho
    del morpho
    return None

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

    def add_morphology(self, morpho):
        if morpho:
            self.morphologies.append(morpho)
            self.loaded_models += 1
            print("Model loaded")
            self.message = "Loading models " + str(int(self.loaded_models))
        global id
        global total
        id += 1
        print(LINE_CLEAR,end="")
        print("\rChecking {:d}/{:d}".format(id,total),end="",flush=True)


    def get_morphos(self, sonata_file, population_name, aabb):
        folder = os.path.dirname(os.path.abspath(sonata_file)) + "/"
        config = libsonata.CircuitConfig.from_file(sonata_file)
        populations = list(config.node_populations)
        if population_name not in populations:
            population_name = populations[0]
        print("Loading population", population_name)
        population = config.node_population(population_name)

        properties = config.node_population_properties(population_name)
        morpho_dir = properties.morphologies_dir
        morpho_ext = ".swc"

        if folder == morpho_dir:
            alternate_dir = properties.alternate_morphology_formats
            if 'h5v1' in alternate_dir:
                morpho_dir = alternate_dir['h5v1'] + "/"
                morpho_ext = ".h5"
            elif 'neurolucida-asc' in alternate_dir:
                morpho_dir = alternate_dir['neurolucida-asc'] + "/"
                morpho_ext = ".asc"

        gids = population.select_all().flatten()
        # gids = gids[0:1000]
        global total
        total = len(gids)
        selection = libsonata.Selection(gids)

        morpho_names = population.get_attribute("morphology", selection)
        print("Number of morphologies {:d}".format(len(morpho_names)))
        xs = population.get_attribute("x", selection)
        ys = population.get_attribute("y", selection)
        zs = population.get_attribute("z", selection)
        rot_xs = population.get_attribute("orientation_x", selection)
        rot_ys = population.get_attribute("orientation_y", selection)
        rot_zs = population.get_attribute("orientation_z", selection)
        rot_ws = population.get_attribute("orientation_w", selection)

        

        # num_iters = 2
        # increment = total//num_iters
        # print(increment)
        
        # for j in range(num_iters+1):
        #     id = j*increment
        #     local_gids = gids[id:id+increment]
        #     local_morpho_names = morpho_names[gids[id:id+increment]]
        #     local_xs = xs[id:id+increment]
        #     local_ys = ys[id:id+increment]
        #     local_zs = zs[id:id+increment]
        #     local_rot_xs = rot_xs[id:id+increment]
        #     local_rot_ys = rot_ys[id:id+increment]
        #     local_rot_zs = rot_zs[id:id+increment]
        #     local_rot_ws = rot_ws[id:id+increment]
        #     with multiprocessing.Pool(multiprocessing.cpu_count() - 2) as pool:
        #         proc = {}
        #         for i, morpho_name in enumerate(local_morpho_names):
        #             morpho_path = morpho_dir + morpho_name + morpho_ext
        #             model = mat4(
        #                 normalize(
        #                     quat(
        #                         local_rot_ws[i],
        #                         local_rot_xs[i],
        #                         local_rot_ys[i],
        #                         local_rot_zs[i])))
        #             model = translate(
        #                 model, vec3(local_xs[i],
        #                             local_ys[i],
        #                             local_zs[i]))
        #             pool.apply_async(
        #                 check_morpho, args=(morpho_path, model, aabb),
        #                 callback=self.add_morphology)
        #         pool.close()
        #         pool.join()
        
        for i, morpho_name in enumerate(morpho_names):
            morpho_path = morpho_dir + morpho_name + morpho_ext
            model = mat4(normalize(quat(rot_ws[i], rot_xs[i], rot_ys[i], rot_zs[i])))
            model = translate(model, vec3(xs[i], ys[i], zs[i]))
            self.add_morphology(check_morpho(morpho_path, model, aabb))
            del morpho_path
            del model

    def add_models(self, sonata_file, population, aabb, lines=False):
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

        self.num_models = 0
        prev_time = time.time()

        self.get_morphos(sonata_file, population, aabb)
        # for path in paths:
        #     self.add_morphology(path)

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
        print("Loaded " + str(self.loaded_models) +
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

    parser.add_argument('sonata_file', help="Circuit file in sonata format")
    parser.add_argument('target', help="Target")
    parser.add_argument('-min', type=int, default=0)
    parser.add_argument('-max', type=int, default=30)
    

    args = parser.parse_args()

    min = vec3(args.min)
    max = vec3(args.max)
    aabb = AABoundingBox()
    aabb.add_pos(min)
    aabb.add_pos(max)


    app = Overlap()
    app.verbose = not args.nfps
    app.set_background()
    app.ks = 2000.0
    app.ksc = 100.0
    app.ksc_factor = 1.05
    app.kd = 0
    app.dt = 0.001


    app.stop_on_solve = not args.cont

    app.add_models(args.sonata_file, args.target, aabb, args.lines)
    app.scene.level = 5
    app.scene.distance = 500
    app.run()
