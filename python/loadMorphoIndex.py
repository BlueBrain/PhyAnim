# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import argparse
import os 
import spatial_index
import sys
from libsonata import *

CIRCUIT_2K = "/gpfs/bbp.cscs.ch/project/proj12/spatial_index/v4/circuit-2k/circuit_config.json"
# INDEX_DIR = "/home/jjgarcia/projects/test-spatial-index/multi_index_2k"
# MORPHOLOGY_DIR = CIRCUIT_2K + "morphologies/ascii/"


if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument("index_dir", help="Directory containing the index")
    parser.add_argument("morphology_dir", help="Directory containing the morphologies")
    parser.add_argument('-min', type=int, default=0)
    parser.add_argument('-max', type=int, default=30)
    args = parser.parse_args()

   


    index = spatial_index.open_index(args.index_dir, max_cache_size_mb=1000)
    prev = time.time()
    min = [args.min, args.min, args.min]
    max = [args.max, args.max, args.max]
    gids = index.box_query(min, max, fields=['gid'])['gid']
    print("Query time: " + "{:.4f}".format(time.time() - prev))
    gids = np.unique(gids)
    print("Number of gids:",len(gids))
    
    
