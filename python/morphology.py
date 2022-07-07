from turtle import position
from render import *
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
        return Vec3(), 1.0

    def add_sample(self, sample: Sample):
        self.samples.append(sample)
        return len(self.samples) - 1

    def samples(self):
        return self._samples

    def add_segment(self, segment: Segment):
        self.segments.append(segment)

    def segments(self):
        return self._segments


def load_swc(path):
    ids = {}
    morpho = Morphology()
    with open(path, 'r') as f:
        for line in f.readlines():
            nums = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", line)
            if len(nums) == 7:
                id = int(nums[0])
                type = int(nums[1])
                position = Vec3(float(nums[2]), float(nums[4]), float(nums[4]))
                radius = float(nums[5])
                parent = int(nums[6])

                if type == 1:
                    sample = Sample(position, radius, SampleType.SOMA)
                    morpho.add_soma_sample(sample)
                if type > 1:
                    sample = Sample(position, radius, SampleType.DENDRITE)
                    sample_id = morpho.add_sample(sample)
                    ids[id] = sample_id
                    if (parent > 1):
                        sample_parent_id = ids.get(parent, 0)
                        morpho.add_segment((sample_id, sample_parent_id))
    return morpho


def mesh_from_morpho(morpho: Morphology):
    lines = []
    triangles = []
    positions = []
    normals = []
    colors = []

    for sample in morpho.samples:
        positions.append(
            [sample.position.x/1000, sample.position.y/1000, sample.position.z/1000])
        normals.append([0, 0, 1])
        colors.append([1.0, 0.5, 0.5])

    for segment in morpho.segments:
        lines.append([segment[0], segment[1]])

    return Mesh(lines, triangles, positions, normals, colors)
