from __future__ import annotations


from glm import *



def lerp(a, b, t):
    return a*(1-t) + b*t


def project_position_segment(p, a, b):
    ba = b - a
    if dot(ba,ba) == 0:
        return (a, 0.5) 
    t = dot((p - a), ba) / dot(ba,ba)
    t = clamp(1,0,t)
    return (lerp(a, b, t), t)


def project_segment_segment(a, b, c, d):
    ba = b-a 
    dc = d-c

    if dot(ba,ba) == 0:
        proj = project_position_segment(a, c, d)
        return (a, 0.5, proj[0], proj[1])
    if dot(dc,dc) == 0:
        proj = project_position_segment(c, a, b)
        return (proj[0], proj[1], c, 0.5)


    dc_sqr_norm = dot(dc, dc)
    a_proj = a - (dc*(dot(a-c, dc)/dc_sqr_norm))
    b_proj = b - (dc*(dot(b-c, dc)/dc_sqr_norm))
    ba_proj = b_proj - a_proj
    if length(ba_proj) < 0.00001:
        t = 0.0
    else:
        t = dot(c-a_proj, ba_proj)/dot(ba_proj, ba_proj)

    ba_p = lerp(a, b, t)
    dc_p = project_position_segment(ba_p, c, d)
    ba_p = project_position_segment(dc_p[0], a, b)
    return (ba_p[0], ba_p[1], dc_p[0], dc_p[1])

