# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import argparse
import os 
import multiprocessing
from random import randint, random
import threading
from time import sleep
import spatial_index
from data.data import *
from render.app import *
import sys
from anim.geometry import *
import libsonata


from multiprocessing import Pool, Process

CIRCUIT_2K = "/gpfs/bbp.cscs.ch/project/proj12/spatial_index/v4/circuit-2k/"
INDEX_DIR = "/gpfs/bbp.cscs.ch/project/proj3/juanjo/multi_index_2k"
MORPHOLOGY_DIR = CIRCUIT_2K + "morphologies/ascii/"


def load_neuron(program, neuron, color):
    try:
        morpho = Morphology(neuron[0], neuron[1], color)
        if program.primitives == GL_LINES:
            mesh = morpho.generate_lines()
        elif program.primitives == GL_PATCHES:
            mesh = morpho.generate_mesh()
        if mesh:
            return (mesh, program, mat4())
    except morphio._morphio.UnknownFileType:
        return None

def get_spatial_indices(index_path, min, max):
    index = spatial_index.open_index(index_path, max_cache_size_mb=1000)
    prev_time = time.time()

    matches = index.box_query(
        min, max,
        fields=['gid', 'morphology', 'x', 'y', 'z', 'orientation_x',
                'orientation_y', 'orientation_z', 'orientation_w'])

    gids =  matches['gid']
    morpho_names = matches["morphology"]
    xs = matches["x"]
    ys = matches["y"]
    zs = matches["z"]
    rot_xs = matches["orientation_x"]
    rot_ys = matches["orientation_y"]
    rot_zs = matches["orientation_z"]
    rot_ws = matches["orientation_w"]


    found_ids = []
    morphos = []
    for i, gid in enumerate(gids):
        if gid not in found_ids:
            found_ids.append(gid)
            morpho_path = MORPHOLOGY_DIR + morpho_names[i] + ".asc"
           
            

            model = mat4(normalize(quat(rot_ws[i],rot_xs[i],rot_ys[i],rot_zs[i])))
            model = translate(vec3(xs[i], ys[i], zs[i])) * model
            morphos.append((morpho_path, model))
    return morphos

class MorphoRenderIndex(App):

    def __init__(self):
        App.__init__(self)
        self.min = vec3()
        self.max = vec3(30)

    def add_models(self, lines = False):
        program = ShaderProgram(
            [("shaders/lines.vert", ShaderType.VERTEX),
             ("shaders/lines.frag", ShaderType.FRAGMENT)], GL_LINES) 
        self.scene.add_model((CubeMesh(vec3(self.min), vec3(self.max)), program, mat4()))

        if not lines:
            program = ShaderProgram(
                [("shaders/quads_tess.vert", ShaderType.VERTEX),
                ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
                ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
                ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES) 

        prev = time.time()
        neurons = get_spatial_indices(INDEX_DIR, self.min, self.max)
        print("{:d} neurons obtained in {:0.2f} s".format(len(neurons), time.time()-prev))
        
        if len(neurons)>0:
            self.num_models = len(neurons)
        prev_time = time.time()
        with Pool(multiprocessing.cpu_count() - 2) as pool:
            for i,neuron in enumerate(neurons):
                pool.apply_async(load_neuron, args=(program, neuron, self.get_color(i)), callback=self.add_model)

            pool.close()
            pool.join()

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
    parser.add_argument('--lines', action='store_true', help="generate morphology meshes as lines")
    parser.add_argument('-min', type=int, default=0)
    parser.add_argument('-max', type=int, default=30)
    args = parser.parse_args()

    app = MorphoRenderIndex()
    app.min = vec3(args.min)
    app.max = vec3(args.max)

    app.set_background()
    app.scene.aabb = AABoundingBox()
    p = threading.Thread(target=app.add_models, 
        args=(args.lines,))
    p.start()
    app.scene.distance = 500.0
    app.scene.level = 5
    app.run()
