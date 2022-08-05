from warnings import catch_warnings
from render.render import *


def compute_normals(positions, triangles):
    normals = []
    # Normals compute
    num_vertex = len(positions)
    num_triangles = len(triangles)
    normals = []
    weights = []
    for i in range(num_vertex):
        normals.append(Vec3(0, 0, 0))
        weights.append(0.0)

    for t in triangles:
        id0 = t.id0
        id1 = t.id1
        id2 = t.id2

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
    return normals


RawTriangle = ((int, int), (int, int), (int, int))


def load_obj(path):
    try:
        raw_positions = []
        raw_normals = []
        raw_triangles = []

        with open(path, 'r') as f:
            readlines = f.readlines()
            for line in readlines:
                words = line.split()
                if len(words) == 0:
                    continue
                if words[0] == 'v':
                    position = Vec3(float(words[1]), float(
                        words[2]), float(words[3]))
                    raw_positions.append(position)
                elif words[0] == 'vn':
                    normal = Vec3(float(words[1]),
                                  float(words[2]), float(words[3]))
                    raw_normals.append(normal)
                elif words[0] == 'f':
                    ids0 = words[1].split('/')
                    ids1 = words[2].split('/')
                    ids2 = words[3].split('/')
                    pos_id0 = int(ids0[0])-1
                    pos_id1 = int(ids1[0])-1
                    pos_id2 = int(ids2[0])-1

                    include_normals = (len(words[1].split('/')) == 3)
                    if include_normals:
                        normal_id0 = int(ids0[2])-1
                        normal_id1 = int(ids1[2])-1
                        normal_id2 = int(ids2[2])-1
                    else:
                        normal_id0 = -1
                        normal_id1 = -1
                        normal_id2 = -1
                    raw_triangles.append((
                        (pos_id0, normal_id0), (pos_id1, normal_id1), (pos_id2, normal_id2)))

        ids = {}
        positions = []
        normals = []
        colors = []
        lines = []
        triangles = []
        quads = []

        for triangle in raw_triangles:
            id0 = ids.get(triangle[0])
            if id0 is None:
                pos_id = triangle[0][0]
                normal_id = triangle[0][1]
                id0 = len(positions)
                positions.append(raw_positions[pos_id])
                ids[triangle[0]] = id0
                if normal_id > -1:
                    normals.append(raw_normals[normal_id])

            id1 = ids.get(triangle[1])
            if id1 is None:
                pos_id = triangle[1][0]
                normal_id = triangle[1][1]
                id1 = len(positions)
                positions.append(raw_positions[pos_id])
                ids[triangle[1]] = id1
                if normal_id > -1:
                    normals.append(raw_normals[normal_id])

            id2 = ids.get(triangle[2])
            if id2 is None:
                pos_id = triangle[2][0]
                normal_id = triangle[2][1]
                id2 = len(positions)
                positions.append(raw_positions[pos_id])
                ids[triangle[2]] = id2
                if normal_id > -1:
                    normals.append(raw_normals[normal_id])
            triangles.append(Triangle(id0, id1, id2))

        for i in range(len(positions)):
            colors.append(Vec3(0.8, 0.2, 0.2))

        if len(normals) == 0:
            normals = compute_normals(positions, triangles)

        mesh = Mesh(lines, triangles, quads, positions, normals, colors)
    except:
        mesh = None
    return mesh
