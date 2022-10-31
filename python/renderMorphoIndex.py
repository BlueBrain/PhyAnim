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

from multiprocessing import Pool, Process

# CIRCUIT_2K = "/gpfs/bbp.cscs.ch/project/proj12/spatial_index/v4/circuit-2k/"
# INDEX_DIR = "/home/jjgarcia/projects/test-spatial-index/multi_index_2k"
# MORPHOLOGY_DIR = CIRCUIT_2K + "morphologies/ascii/"

def unique_gids(matches):
    gids = matches['gid']
    np.unique(gids)
    return gids

def unique_neurons(matches):
    gids = matches['gid']
    morphologies = matches['morphology']
    x = matches['x']
    y = matches['y']
    z = matches['z']
    rx = matches['orientation_x']
    ry = matches['orientation_y']
    rz = matches['orientation_z']
    gids_map = {}
    for i, gid in enumerate(gids):
        if gid not in gids_map:
            gids_map[gid] = (gid, morphologies[i], Vec3(
                x[i], y[i], z[i]), Vec3(rx[i], ry[i], rz[i]))
    return gids_map.values()

def load_neuron(program, neuron, morphology_dir, color):
    try:
        path = None
        if os.path.exists(morphology_dir + neuron[1] + ".asc"):
            path = morphology_dir + neuron[1] + ".asc"
        elif os.path.exists(morphology_dir + neuron[1] + ".swc"):
            path = morphology_dir + neuron[1] + ".swc"
        elif os.path.exists(morphology_dir + neuron[1] + ".h5"):
            path = morphology_dir + neuron[1] + ".h5"
        if path:
            morpho = Morphology(path, color)
            if program.primitives == GL_LINES:
                mesh = morpho.generate_lines()
            elif program.primitives == GL_PATCHES:
                mesh = morpho.generate_mesh()
            if mesh:
                p = neuron[2]
                r = neuron[3]
                modelMat = mat4_from_rotation(r)
                modelMat.translate(p)
                return (mesh, program, modelMat)
        return None
    except morphio._morphio.UnknownFileType:
        return None

class MorphoRenderIndex(App):

    def __init__(self):
        App.__init__(self)
        self.min = 0
        self.max = 30

    def add_models(self, index_dir, morphology_dir, lines = False):
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
 
        index = spatial_index.open_index(index_dir, max_cache_size_mb=1000)
        prev_time = time.time()

        neurons = unique_neurons(
            index.box_query(
                [self.min, self.min, self.min], [self.max, self.max, self.max],
                fields=['gid', 'morphology', 'x', 'y', 'z', 'orientation_x',
                        'orientation_y', 'orientation_z']))
        print("\rNumber of neurons: " + str(len(neurons)) + " loaded from index in " + 
            "{:.2f}".format(time.time() - prev_time) + " seconds.")

        self.num_models = len(neurons)
        prev_time = time.time()
        
        with Pool(multiprocessing.cpu_count() - 2) as pool:
            for i,neuron in enumerate(neurons):
                pool.apply_async(load_neuron, args=(program,neuron,morphology_dir,self.get_color(i)), callback=self.add_model)

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
            "{:.2f}".format(time.time() - prev_time) + " seconds." )

    

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument("index_dir", help="Directory containing the index")
    parser.add_argument("morphology_dir", help="Directory containing the morphologies")
    parser.add_argument('--lines', action='store_true', help="generate morphology meshes as lines")
    parser.add_argument('-min', type=int, default=0)
    parser.add_argument('-max', type=int, default=30)
    args = parser.parse_args()

    app = MorphoRenderIndex()
    app.min = args.min
    app.max = args.max
    app.set_background()
    app.scene.aabb = AABoundingBox()
    p = threading.Thread(target=app.add_models, args=(args.index_dir,args.morphology_dir, args.lines,))
    p.start()
    app.scene.distance = 500.0
    app.scene.level = 5
    app.run()
