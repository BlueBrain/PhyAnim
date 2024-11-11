import spatial_index
from glm import *
import argparse


import numpy as np


parser = argparse.ArgumentParser()
parser.add_argument("index_file", help="Directory containing the index")
parser.add_argument('min_x', type=float)
parser.add_argument('min_y', type=float)
parser.add_argument('min_z', type=float)
parser.add_argument('max_x', type=float)
parser.add_argument('max_y', type=float)
parser.add_argument('max_z', type=float)
args = parser.parse_args()



index = spatial_index.open_index(args.index_file, max_cache_size_mb=1000)
matches = index.box_query([args.min_x,args.min_y,args.min_z], [args.max_x,args.max_y,args.max_z], fields=['gid'])

ids = np.unique(matches['gid'])

for id in ids:
    print(id,end=" ")
print()

print("Number of neurons", len(ids) )