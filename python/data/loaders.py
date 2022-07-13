from render.render import *


def load_obj(path):

    positions = []
    normals = []
    colors = []
    lines = []
    triangles = []

    with open(path, 'r') as f:
        readlines = f.readlines()
        for line in readlines:
            words = line.split()
            if len(words) == 0:
                continue
            if words[0] == 'v':
                position = Vec3(float(words[1]), float(
                    words[2]), float(words[3]))
                positions.append(position)
            elif words[0] == 'vn':
                normal = Vec3(float(words[1]),
                              float(words[2]), float(words[3]))
                normals.append(normal)
            elif words[0] == 'f':
                triangles += [int(words[1].split('/')[0])-1,
                              int(words[2].split('/')[0])-1,
                              int(words[3].split('/')[0])-1]

    for i in range(len(positions)):
        colors.append(Vec3(0.8, 0.2, 0.2))

    # Normals compute
    if (len(normals) == 0):
        num_vertex = len(positions)
        num_triangles = int(len(triangles)/3)
        normals = []
        weights = []
        for i in range(num_vertex):
            normals.append(Vec3(0, 0, 0))
            weights.append(0.0)

        for i in range(num_triangles):
            id0 = triangles[i*3]
            id1 = triangles[i*3+1]
            id2 = triangles[i*3+2]

            p0 = positions[id0]
            p1 = positions[id1]
            p2 = positions[id2]
            axis0 = (p1-p0).normalized()
            axis1 = (p2-p0).normalized()
            normal = axis0.cross(axis1)
            normals[id0] += normal
            normals[id1] += normal
            normals[id2] += normal
            weights[id0] += 1
            weights[id1] += 1
            weights[id2] += 1

        for i in range(num_vertex):
            normals[i] /= weights[i]

    return Mesh(lines, triangles, positions, normals, colors)
