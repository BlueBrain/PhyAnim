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

        # # mean radius
        radius = 0.0
        for sample in self.soma_samples:
            # radius += (sample.position - center).norm()
            radius += sample.radius
        radius /= len(self.soma_samples)
        # print(radius)

        # # max radius
        # radius = 0
        # for sample in self.soma_samples:
        #     radius = max(radius, sample.radius)

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
                        # morpho.add_soma_sample(sample)
                        neurites.append(sample_id)
                    else:
                        sample_parent_id = ids.get(parent, 0)
                        morpho.add_segment((sample_id, sample_parent_id))
                else:
                    sample = Sample(position, radius, SampleType.SOMA)
                    morpho.add_soma_sample(sample)

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
    color = Vec3(0, 0.5, 0.5)
    center, radius = morpho.soma_center_radius()
    sphere = Sphere(center, radius)
    (positions, normals, colors) = sphere.get_geometry(color)
    triangles = sphere.get_triangles()
    quads = sphere.get_quads()
    return Mesh([], triangles, quads, positions, normals, colors)


def mesh_from_morpho(morpho: Morphology):
    lines = []
    quads = []
    triangles = []
    positions = []
    normals = []
    colors = []

    segments = morpho.get_segments()
    color = Vec3(0.5, 1, 0.5)

    id = 0
    for seg in segments:
        capsule = Capsule(seg[0].position, seg[0].radius,
                          seg[1].position, seg[1].radius)
        (pos, norms, cs) = capsule.get_geometry(color)
        positions += pos
        normals += norms
        colors += cs
        triangles += capsule.get_triangles(id)
        quads += capsule.get_quads(id)
        id += len(pos)
    return Mesh(lines, triangles, quads, positions, normals, colors)
