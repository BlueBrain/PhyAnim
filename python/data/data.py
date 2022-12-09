from data.loaders import *

from geometry.geometry import *
import spatial_index
import morphio.mut
import libsonata
import os




def get_spatial_indices(index_path, min, max):
    index = spatial_index.open_index(index_path, max_cache_size_mb=1000)
    matches = index.box_query(min, max, fields=['gid'])
    return np.unique(matches['gid'])

def get_morpho(morpho_path):
    return morphio.mut.Morphology(morpho_path)

def get_morpho_model(sonata_file: str, population_name=None, gids=[]):
    neurons = []
    try:
        folder = os.path.dirname(os.path.abspath(sonata_file)) + "/"
        config = libsonata.CircuitConfig.from_file(sonata_file)
        populations = list(config.node_populations)
        if population_name not in populations:
            population_name = populations[0]
        # print("Loading population", population_name)
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

        if len(gids) > 0:
            selection = libsonata.Selection(gids)
            morpho_names = population.get_attribute("morphology", selection)
            xs = population.get_attribute("x", selection)
            ys = population.get_attribute("y", selection)
            zs = population.get_attribute("z", selection)
            rot_xs = population.get_attribute("orientation_x", selection)
            rot_ys = population.get_attribute("orientation_y", selection)
            rot_zs = population.get_attribute("orientation_z", selection)
            rot_ws = population.get_attribute("orientation_w", selection)

            for i, morpho_name in enumerate(morpho_names):
                morpho_path = morpho_dir + morpho_name + morpho_ext
                model = translate(vec3(xs[i], ys[i], zs[i])) * mat4(
                    normalize(quat(rot_ws[i], rot_xs[i], rot_ys[i], rot_zs[i])))
                neurons.append((morpho_path, model))
    except:
        pass
    return neurons


