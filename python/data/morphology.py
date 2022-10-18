from render.render import *
from enum import Enum
import re
import morphio



def lines_from_morpho(morpho: morphio.Morphology):
    lines = []
    triangles = []
    quads = []
    positions = []
    normals = []
    colors = []
    id = 0
    for section in morpho.sections:
        for i in range(len(section.points)):
            radius = section.diameters[i] * 0.5
            point = Vec3(section.points[i])
            positions.append(point)
            normals.append(Vec3(0, 0, 1))
            colors.append(Vec3(0.5, 1, 0.5))
            if i>0:
                lines.append(Line(id-1, id))
            id += 1
    return Mesh(lines, triangles, quads, positions, normals, colors)


def mesh_from_morpho(morpho: morphio.Morphology):
    lines = []
    triangles = []
    quads = []
    positions = []
    normals = []
    colors = []
    id = 0

    center = Vec3(morpho.soma.center)
    radius = morpho.soma.max_distance
    # min_dist = 1e24
    # # for sec in morpho.root_sections:
    # #     center += Vec3(sec.points[0])
    # # center /= len(morpho.root_sections)
    # for sec in morpho.root_sections:
    #     dist = distance(center, Vec3(sec.points[0]))
    #     min_dist = min(min_dist, dist)
    sphere = Sphere(center, radius)

    (pos, norms, cs) = sphere.get_geometry(Vec3(0.5, 1, 0.5))
    positions += pos
    normals += norms
    colors += cs
    quads += sphere.get_quads(id)
    id += len(pos)

    for section in morpho.sections:
        for i in range(1,len(section.points)):
            r0 = section.diameters[i-1] * 0.5
            p0 = Vec3(section.points[i-1])
            r1 = section.diameters[i] * 0.5
            p1 = Vec3(section.points[i])
            
            capsule = Capsule(p0, r0, p1, r1)
            (pos, norms, cs) = capsule.get_geometry(Vec3(0.5, 1, 0.5))
            positions += pos
            normals += norms
            colors += cs
            quads += capsule.get_quads(id)
            id += len(pos)


    mesh = Mesh(lines, triangles, quads, positions, normals, colors)
    mesh.aabb = AABoundingBox()
    mesh.aabb.add_pos(center - Vec3(radius * 2.0))
    mesh.aabb.add_pos(center + Vec3(radius * 2.0))
    return mesh

# def soma_mesh_from_morpho(morpho: Morphology):
#     color = Vec3(0, 0.5, 0.5)
#     center, radius = morpho.soma_center_radius()
#     sphere = Sphere(center, radius)
#     (positions, normals, colors) = sphere.get_geometry(color)
#     triangles = sphere.get_triangles()
#     quads = sphere.get_quads()
#     return Mesh([], triangles, quads, positions, normals, colors)


# def mesh_from_morpho(morpho: Morphology):
#     lines = []
#     quads = []
#     triangles = []
#     positions = []
#     normals = []
#     colors = []

#     segments = morpho.get_segments()
#     color = Vec3(0.5, 1, 0.5)

#     id = 0
#     for seg in segments:
#         capsule = Capsule(seg[0].position, seg[0].radius,
#                           seg[1].position, seg[1].radius)
#         (pos, norms, cs) = capsule.get_geometry(color)
#         positions += pos
#         normals += norms
#         colors += cs
#         triangles += capsule.get_triangles(id)
#         quads += capsule.get_quads(id)
#         id += len(pos)
#     return Mesh(lines, triangles, quads, positions, normals, colors)
