python renderCircuit.py  \
 /gpfs/bbp.cscs.ch/project/proj83/jira-tickets/NSETM-1948-extract-hex-O1/data/S1_data/circuit_config.json  S1nonbarrel_neurons \
 -min_x 3000 -max_x 3100 \
 -min_y 1000 -max_y 1100 \
 -min_z -1000 -max_z -900 \
 -lines -n 100 $*
