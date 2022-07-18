from render.math import *
from render.render import *
from enum import Enum
import re


class SampleType(Enum):
    SOMA = 0
    DENDRITE = 1


class Sample:
    def __init__(self, position: Vec3, radius: float, type: SampleType = SampleType.DENDRITE):
        self.position = position
        self.radius = radius
        self.type = type

    @property
    def position(self):
        return self._position

    @position.setter
    def position(self, position: Vec3):
        self._position = position

    @property
    def radius(self):
        return self._radius

    @radius.setter
    def radius(self, radius: float):
        self._radius = radius

    @property
    def type(self):
        return self._type

    @type.setter
    def type(self, type: SampleType):
        self._type = type

    def __str__(self):
        out = "Type: " + str(self.type) + " , position: " + \
            str(self.position) + " , radius: " + str(self.radius)
        return out


Segment = (int32_t, int32_t)


class Morphology:
    def __init__(self):
        self.soma_samples = []
        self.samples = []
        self.segments = []

    def add_soma_sample(self, sample: Sample):
        self.soma_samples.append(sample)

    def soma_center_radius(self):
        if len(self.soma_samples) == 0:
            return Vec3(), 1.0
        center = Vec3()
        for sample in self.soma_samples:
            center += sample.position
        center /= len(self.soma_samples)
        radius = 0.0
        for sample in self.soma_samples:
            radius += (sample.position - center).norm()
        radius /= len(self.soma_samples)
        return center, radius

    def add_sample(self, sample: Sample):
        self.samples.append(sample)
        return len(self.samples) - 1

    def samples(self):
        return self._samples

    def add_segment(self, segment: Segment):
        self.segments.append(segment)

    def segments(self):
        return self._segments

    def get_segments(self):
        segs = []
        for seg in self.segments:
            segs.append((self.samples[seg[0]], self.samples[seg[1]]))
        return segs


def load_swc(path):
    ids = {}
    morpho = Morphology()
    neurites = []
    with open(path, 'r') as f:
        for line in f.readlines():
            if line[0] == '#':
                continue
            nums = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", line)
            if len(nums) == 7:
                id = int(nums[0])
                type = int(nums[1])
                position = Vec3(float(nums[2]), float(nums[3]), float(nums[4]))
                radius = float(nums[5])
                parent = int(nums[6])

                if type != 1:
                    sample = Sample(position, radius, SampleType.DENDRITE)
                    sample_id = morpho.add_sample(sample)
                    ids[id] = sample_id
                    if (parent == 1):
                        morpho.add_soma_sample(sample)
                        neurites.append(sample_id)
                    else:
                        sample_parent_id = ids.get(parent, 0)
                        morpho.add_segment((sample_id, sample_parent_id))

    center, radius = morpho.soma_center_radius()
    for neurite_id in neurites:
        sample = morpho.samples[neurite_id]
        soma_sample = Sample(center, sample.radius, SampleType.DENDRITE)
        sample_id = morpho.add_sample(soma_sample)
        morpho.add_segment((sample_id, neurite_id))

    return morpho


def lines_from_morpho(morpho: Morphology):
    lines = []
    triangles = []
    positions = []
    normals = []
    colors = []

    for sample in morpho.samples:
        positions.append(sample.position)
        normals.append(Vec3(0, 0, 1))
        colors.append(Vec3(0.5, 1, 0.5))

    for segment in morpho.segments:
        lines.append(Line(segment[0], segment[1]))

    return Mesh(lines, triangles, positions, normals, colors)


def soma_mesh_from_morpho(morpho: Morphology):
    lines = []
    triangles = []
    positions = []
    normals = []
    colors = []

    color = Vec3(0, 0.5, 0.5)

    center, radius = morpho.soma_center_radius()
    x = Vec3(1, 0, 0) * radius
    y = Vec3(0, 1, 0) * radius
    z = Vec3(0, 0, 1) * radius

    positions += [center + x, center - x,
                  center + y, center - y,
                  center + z, center - z]
    normals += [x, x*-1, y, y*-1, z, z*-1]
    colors += [color, color, color, color, color, color]
    triangles += [Triangle(0, 2, 4), Triangle(0, 4, 3), Triangle(0, 3, 5), Triangle(0, 5, 2),
                  Triangle(1, 4, 2), Triangle(1, 3, 4), Triangle(1, 5, 3), Triangle(1, 2, 5)]
    return Mesh(lines, triangles, positions, normals, colors)


def mesh_from_morpho(morpho: Morphology):
    lines = []

    segments = morpho.get_segments()
    num_segments = len(segments)

    triangles = [None] * (num_segments * 16)
    positions = [None] * (num_segments * 10)
    normals = [None] * (num_segments * 10)
    colors = [None] * (num_segments * 10)

    color = Vec3(0.5, 1, 0.5)

    pos_offset_inc = 10
    triangles_offset_inc = 16

    pos_offset = 0
    triangles_offset = 0
    seg_id = 0.0
    center, radius = morpho.soma_center_radius()
    for seg in segments:
        # alpha = (seg[0].position - center).norm()/100.0
        # c = alpha*0.5+0.5
        # color = Vec3(0, c, c)
        color = Vec3(0.5, 1, 0.5)
        x = (seg[1].position - seg[0].position).normalized()
        if x.norm() == 0:
            x = Vec3(1, 0, 0)
        if x.y == 1.0 or x.y == -1.0:
            y = Vec3(-1, 0, 0)
        else:
            y = Vec3(0, 1, 0)
        z = (x.cross(y)).normalized()
        y = (z.cross(x)).normalized()

        id0 = pos_offset
        id1 = id0 + 1
        id2 = id0 + 2
        id3 = id0 + 3
        id4 = id0 + 4
        id5 = id0 + 5
        id6 = id0 + 6
        id7 = id0 + 7
        id8 = id0 + 8
        id9 = id0 + 9
        positions[pos_offset] = seg[0].position + y * seg[0].radius
        positions[pos_offset+1] = seg[0].position + z * seg[0].radius
        positions[pos_offset+2] = seg[0].position - y * seg[0].radius
        positions[pos_offset+3] = seg[0].position - z * seg[0].radius
        positions[pos_offset+4] = seg[1].position + y * seg[1].radius
        positions[pos_offset+5] = seg[1].position + z * seg[1].radius
        positions[pos_offset+6] = seg[1].position - y * seg[1].radius
        positions[pos_offset+7] = seg[1].position - z * seg[1].radius
        positions[pos_offset+8] = seg[0].position - x * seg[0].radius
        positions[pos_offset+9] = seg[1].position + x * seg[1].radius
        normals[pos_offset] = y*seg[0].radius
        normals[pos_offset+1] = z*seg[0].radius
        normals[pos_offset+2] = y*-1*seg[0].radius
        normals[pos_offset+3] = z*-1*seg[0].radius
        normals[pos_offset+4] = y*seg[0].radius
        normals[pos_offset+5] = z*seg[0].radius
        normals[pos_offset+6] = y*-1*seg[0].radius
        normals[pos_offset+7] = z*-1*seg[0].radius
        normals[pos_offset+8] = x*-1*seg[0].radius
        normals[pos_offset+9] = x*seg[1].radius
        colors[pos_offset] = color
        colors[pos_offset+1] = color
        colors[pos_offset+2] = color
        colors[pos_offset+3] = color
        colors[pos_offset+4] = color
        colors[pos_offset+5] = color
        colors[pos_offset+6] = color
        colors[pos_offset+7] = color
        colors[pos_offset+8] = color
        colors[pos_offset+9] = color
        triangles[triangles_offset] = Triangle(id0, id1, id5)
        triangles[triangles_offset+1] = Triangle(id0, id5, id4)
        triangles[triangles_offset+2] = Triangle(id1, id2, id6)
        triangles[triangles_offset+3] = Triangle(id1, id6, id5)
        triangles[triangles_offset+4] = Triangle(id2, id3, id7)
        triangles[triangles_offset+5] = Triangle(id2, id7, id6)
        triangles[triangles_offset+6] = Triangle(id3, id0, id4)
        triangles[triangles_offset+7] = Triangle(id3, id4, id7)
        triangles[triangles_offset+8] = Triangle(id8, id1, id0)
        triangles[triangles_offset+9] = Triangle(id8, id2, id1)
        triangles[triangles_offset+10] = Triangle(id8, id3, id2)
        triangles[triangles_offset+11] = Triangle(id8, id0, id3)
        triangles[triangles_offset+12] = Triangle(id9, id4, id5)
        triangles[triangles_offset+13] = Triangle(id9, id5, id6)
        triangles[triangles_offset+14] = Triangle(id9, id6, id7)
        triangles[triangles_offset+15] = Triangle(id9, id7, id4)
        pos_offset += pos_offset_inc
        triangles_offset += triangles_offset_inc
        seg_id += 1
    return Mesh(lines, triangles, positions, normals, colors)
