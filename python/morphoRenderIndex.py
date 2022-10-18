# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

from gc import is_finalized
import multiprocessing
from random import randint, random
import threading
from time import sleep
import spatial_index
from data.data import *
from render.app import *
import sys

from multiprocessing import Pool, Process

CIRCUIT_2K = "/gpfs/bbp.cscs.ch/project/proj12/spatial_index/v4/circuit-2k/"
INDEX_DIR = "/home/jjgarcia/projects/test-spatial-index/multi_index_2k"
MORPHOLOGY_DIR = CIRCUIT_2K + "morphologies/ascii/"

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

def load_neuron(program, neuron):
    path = MORPHOLOGY_DIR + neuron[1] + ".asc"
    morpho = morphio.Morphology(path)
    mesh = mesh_from_morpho(morpho)
    p = neuron[2]
    r = neuron[3]
    modelMat = mat4_from_rotation(r)
    modelMat.translate(p)
    return (mesh, program, modelMat)

def load_neuron_lines(program, neuron):
    path = MORPHOLOGY_DIR + neuron[1] + ".asc"
    try:
        morpho = morphio.Morphology(path)
        mesh = lines_from_morpho(morpho)
        p = neuron[2]
        r = neuron[3]
        modelMat = mat4_from_rotation(r)
        modelMat.translate(p)
        return (mesh, program, modelMat)
    except:
        return None
class MorphoRenderIndex(App):

    def __init__(self):
        App.__init__(self)
        self.min = 0
        self.max = 30

    def add_model(self, model):
        if model:
            self.scene.add_model(model)
            self.mesh_loaded += 1
        self.processed += 1
        self.message = "Loading meshes " + str(int(self.processed/self.num_neurons*100))+"%"


    def add_models(self, meshes):
        if meshes:
            program = ShaderProgram(
                [("shaders/quads_tess.vert", ShaderType.VERTEX),
                ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
                ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
                ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)
        else:
            program = ShaderProgram(
                [("shaders/lines.vert", ShaderType.VERTEX),
                ("shaders/lines.frag", ShaderType.FRAGMENT)], GL_LINES)
 
        index = spatial_index.open_index(INDEX_DIR, max_cache_size_mb=1000)
        prev_time = time.time()
        neurons = unique_neurons(
            index.box_query(
                [self.min, self.min, self.min], [self.max, self.max, self.max],
                fields=['gid', 'morphology', 'x', 'y', 'z', 'orientation_x',
                        'orientation_y', 'orientation_z']))
        print("\rNumber of neurons: " + str(len(neurons)) + " loaded from index in " + 
            "{:.2f}".format(time.time() - prev_time) + " seconds.")

        self.num_neurons = len(neurons)
        self.processed = 0
        self.mesh_loaded = 0
        prev_time = time.time()
        
        with Pool(multiprocessing.cpu_count() - 2) as pool:
            for neuron in neurons:
                if meshes:
                    pool.apply_async(load_neuron, args=(program,neuron,), callback=self.add_model)
                else:
                    pool.apply_async(load_neuron_lines, args=(program, neuron,),
                        callback=self.add_model)

            pool.close()
            pool.join()
        
        self.message = ""
        num_lines = 0
        num_triangles = 0
        for model in self.scene.models:
            num_lines += model[0].num_lines/2
            num_triangles += model[0].num_triangles/3
            num_triangles += model[0].num_quads/2
        print("\rLoaded " + str(self.mesh_loaded) + "/" + str(len(neurons)) + 
            " meshes with " + str(num_lines/1000.0) + "K lines and " +
            str(num_triangles/1000.0) + "K triangles in " +
            "{:.2f}".format(time.time() - prev_time) + " seconds." )

    

if __name__ == "__main__":
    
    meshes = True
    min = 0.0
    max = 30.0
    args = sys.argv[1:]
    for i, arg in enumerate(args):
        if arg == "-min":
            min = float(args[i+1])
        elif arg == "-max":
            max = float(args[i+1])
        elif arg == "-lines":
            meshes = False

    app = MorphoRenderIndex()
    app.min = min
    app.max = max
    app.set_background()
    app.scene.aabb = AABoundingBox()
    p = threading.Thread(target=app.add_models, args=(meshes,))
    p.start()
    app.scene.distance = 500.0
    app.scene.level = 5
    app.run()
