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


def get_neurons(sonata_file: str, population_name=None, ids=None):
    try:
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

        if len(ids) > 0:
            selection = libsonata.Selection(ids)
        else:
            ids = population.select_all().flatten()
            print("All",len(ids))
            selection = libsonata.Selection(ids[-11:-1])


        morpho_names = population.get_attribute("morphology", selection)
        xs = population.get_attribute("x", selection)
        ys = population.get_attribute("y", selection)
        zs = population.get_attribute("z", selection)
        rot_xs = population.get_attribute("orientation_x", selection)
        rot_ys = population.get_attribute("orientation_y", selection)
        rot_zs = population.get_attribute("orientation_z", selection)
        rot_ws = population.get_attribute("orientation_w", selection)

        morphos = []
        for i, morpho_name in enumerate(morpho_names):
            
            morpho_path = morpho_dir + morpho_name + morpho_ext
            model = mat4(normalize(quat(rot_ws[i],rot_xs[i],rot_ys[i],rot_zs[i])))
            model = translate(model, vec3(xs[i], ys[i], zs[i]))
            morphos.append((morpho_path, model))
        return morphos
    except:
        return None

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
    matches = index.box_query(min, max, fields=['gid'])
    return np.unique(matches['gid'])
    
class MorphoRenderIndex(App):

    def __init__(self):
        App.__init__(self)
        self.min = vec3()
        self.max = vec3(30)

    def add_models(self, index_file, sonata_file, population_name, lines = False):
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

        gids = []
        prev = time.time()
        gids = get_spatial_indices(index_file, self.min, self.max)
        print("{:d} neurons loaded in {:0.2f} s".format(len(gids), time.time()-prev))

        neurons = []
        neurons = get_neurons(sonata_file, population_name, gids)
        if neurons:
            self.num_models = len(neurons)

        prev_time = time.time()        
        with Pool(multiprocessing.cpu_count() - 2) as pool:
            for i,neuron in enumerate(neurons):
                pool.apply_async(load_neuron, args=(program,neuron,self.get_color(i)), callback=self.add_model)
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
    parser.add_argument("index_file", help="Directory containing the index")
    parser.add_argument("sonata_file", help="Sonata file")
    parser.add_argument("population_name", help="Population name")
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
        args=(args.index_file, args.sonata_file, args.population_name, args.lines,))
    p.start()
    app.scene.distance = 500.0
    app.scene.level = 5
    app.run()
