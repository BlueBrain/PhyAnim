# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

# import argparse
# import os 
# import sys
# from anim.massSpring import *
# import spatial_index
# import libsonata

# LINE_CLEAR = '\x1b[2K'


# # CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj12/spatial_index/v4/circuit-2k/circuit_config.json"
# # CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj83/jira-tickets/NSETM-1948-extract-hex-O1/data/S1_data/circuit_config.json"
# # INDEX_DIR = "/home/jjgarcia/projects/test-spatial-index/multi_index_2k"
# # MORPHOLOGY_DIR = CIRCUIT_2K + "morphologies/ascii/"
# # TARGET = "S1nonbarrel_neurons"

# gids = []
# total = 0
# id = 0

# def add_gids(gid):
#     global gids
#     if gid >= 0:
#         gids.append(gid)
#         print(gid)
#     global id
#     global total
#     id += 1
#     print(LINE_CLEAR,end="")
#     print("\rChecking {:d}/{:d}".format(id,total),end="",flush=True)

# def check_morpho(gid, morpho_path, model, aabb):
#     morpho = Morphology(morpho_path, model)
#     result = -1
#     if collide(morpho, aabb):
#         print(morpho_path, end=" ", flush=True)
#         result = gid
#     del morpho
#     return result
        
# def get_gids(sonata_file, population_name, aabb):
#     folder = os.path.dirname(os.path.abspath(sonata_file)) + "/"
#     config = libsonata.CircuitConfig.from_file(sonata_file)
#     populations = list(config.node_populations)
#     if population_name not in populations:
#         population_name = populations[0]
#     print("Loading population", population_name)
#     population = config.node_population(population_name)

#     properties = config.node_population_properties(population_name)
#     morpho_dir = properties.morphologies_dir
#     morpho_ext = ".swc"

#     if folder == morpho_dir:
#         alternate_dir = properties.alternate_morphology_formats
#         if 'h5v1' in alternate_dir:
#             morpho_dir = alternate_dir['h5v1'] + "/"
#             morpho_ext = ".h5"
#         elif 'neurolucida-asc' in alternate_dir:
#             morpho_dir = alternate_dir['neurolucida-asc'] + "/"
#             morpho_ext = ".asc"

#     gids = population.select_all().flatten()
#     # gids = gids[0:1000]
#     global total
#     total = len(gids)
#     selection = libsonata.Selection(gids)

#     morpho_names = population.get_attribute("morphology", selection)
#     print("Number of morphologies {:d}".format(len(morpho_names)))
#     xs = population.get_attribute("x", selection)
#     ys = population.get_attribute("y", selection)
#     zs = population.get_attribute("z", selection)
#     rot_xs = population.get_attribute("orientation_x", selection)
#     rot_ys = population.get_attribute("orientation_y", selection)
#     rot_zs = population.get_attribute("orientation_z", selection)
#     rot_ws = population.get_attribute("orientation_w", selection)

    

#     num_iters = 2
#     increment = total//num_iters
#     print(increment)
    
#     for j in range(num_iters+1):
#         id = j*increment
#         local_gids = gids[id:id+increment]
#         local_morpho_names = morpho_names[gids[id:id+increment]]
#         local_xs = xs[id:id+increment]
#         local_ys = ys[id:id+increment]
#         local_zs = zs[id:id+increment]
#         local_rot_xs = rot_xs[id:id+increment]
#         local_rot_ys = rot_ys[id:id+increment]
#         local_rot_zs = rot_zs[id:id+increment]
#         local_rot_ws = rot_ws[id:id+increment]
#         with multiprocessing.Pool(multiprocessing.cpu_count() - 2) as pool:
#             proc = {}
#             for i, morpho_name in enumerate(local_morpho_names):
#                 morpho_path = morpho_dir + morpho_name + morpho_ext
#                 model = mat4(
#                     normalize(
#                         quat(
#                             local_rot_ws[i],
#                             local_rot_xs[i],
#                             local_rot_ys[i],
#                             local_rot_zs[i])))
#                 model = translate(
#                     model, vec3(local_xs[i],
#                                 local_ys[i],
#                                 local_zs[i]))
#                 pool.apply_async(
#                     check_morpho, args=(local_gids[i],
#                                         morpho_path, model, aabb),
#                     callback=add_gids)
#             pool.close()
#             pool.join()
    
#     # for i, morpho_name in enumerate(morpho_names):
#     #     morpho_path = morpho_dir + morpho_name + morpho_ext
#     #     model = mat4(normalize(quat(rot_ws[i], rot_xs[i], rot_ys[i], rot_zs[i])))
#     #     model = translate(model, vec3(xs[i], ys[i], zs[i]))
#     #     add_gids(check_morpho(gids[i],morpho_path, model, aabb))
#     #     del morpho_path
#     #     del model
    
#     print()
    

# if __name__ == "__main__":
   

    # parser = argparse.ArgumentParser()
    # parser.add_argument('index_folder', help="Circuit file in sonata format")
    # parser.add_argument('sonata_file', help="Circuit file in sonata format")
    # parser.add_argument('target', help="Target")
    # parser.add_argument('-min_x', type=float, default=0)
    # parser.add_argument('-min_y', type=float, default=0)
    # parser.add_argument('-min_z', type=float, default=0)
    # parser.add_argument('-max_x', type=float, default=100)
    # parser.add_argument('-max_y', type=float, default=100)
    # parser.add_argument('-max_z', type=float, default=100)
    
    # args = parser.parse_args()
    
    # min = vec3(args.min_x, args.min_y, args.min_z)
    # max = vec3(args.max_x, args.max_y, args.max_z)

    # index_folder = "/gpfs/bbp.cscs.ch/project/proj3/juanjo/multi_index_nsetm"
    
import spatial_index
import time
import numpy as np

# INDEX_DIR = "/gpfs/bbp.cscs.ch/project/proj3/juanjo/multi_index_2k"
INDEX_DIR = "/gpfs/bbp.cscs.ch/project/proj3/juanjo/multi_index_nsetm"

index = spatial_index.open_index(INDEX_DIR, max_cache_size_mb=1000)

# matches = index.box_query([0, 0, 0], [0, 0, 0], fields=['gid'])
# print(matches['gid'])
prev = time.time()
matches = index.box_query(
    [0, 0, 0],
    [0, 0, 0],
    fields=['gid'])
elapsed = time.time() - prev
print(len(matches['gid']))
gids = np.unique(matches['gid'])
print("{:d} neurons obtained in {:2f} seconds".format(len(gids),elapsed))





    # print("\nQuery [gids]:")
    # matches = index.box_query(min, max, fields=['gid'])
    # print("\tgids:",matches['gid'])

    # print("\nQuery [gids,x]:")
    # print("\tgids:",matches['gid'])
  
    # matches = index.box_query(
    #     min, max,
    #     fields=['gid', 'morphology', 'x', 'y', 'z', 'orientation_x',
    #             'orientation_y', 'orientation_z', 'orientation_w'])

    # matches = index.box_query(
    #     min, max,
    #     fields=['gid', 'x', 'y', 'z', 'orientation_x',
    #             'orientation_y', 'orientation_z', 'orientation_w'])

    # matches = index.box_query(
    #     min, max,
    #     fields=['gid'])

    # print(matches['gid'])

    # print(len(matches['gid']))

    # print(matches['morphology'])









