# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import copy
from turtle import position
import glfw
from numpy import float32
from render import *
from morphology import *
import sys


class MorphoRender:

    def __init__(self, path):
        self.width = 600
        self.height = 600

        if not self.init_window():
            return None
        glfw.set_key_callback(self.window, self.key_callback)
        glfw.set_window_size_callback(self.window, self.resize_callback)
        glfw.make_context_current(self.window)
        # # print(glGetString(GL_VERSION))
        self.engine = Engine(self.width, self.height)

        # self.engine.scene.meshes.append(Quad())

        self.engine.scene.meshes.append(
            mesh_from_morpho(load_swc(path)))

    def init_window(self):
        if not glfw.init():
            return False

        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

        self.window = glfw.create_window(
            self.width, self.height, "Morphology Render", None, None)

        if self.window is None:
            print("Window creation failed")
            glfw.terminate()
            return False

        print("Window created with size " +
              str(self.width) + "x" + str(self.height))
        return True

    def run(self):
        glfw.make_context_current(self.window)

        t = .0
        while not glfw.window_should_close(self.window):
            glfw.poll_events()

            self.engine.background_color = Vec3(
                math.sin(t)*0.2+0.8, math.cos(t)*0.2+0.8, 1)
            # self.engine.scene.meshes = [
            #     Mesh([0, 1], [], [0, 0, 0, 0, 1, 0], [], [0, 0, 0, 0, 0, 0])]
            self.engine.render(t)
            t += 0.01
            glfw.swap_buffers(self.window)
        glfw.terminate()

    def key_callback(self, window, key, scancode, action, mods):
        if key == glfw.KEY_SPACE:
            if action == glfw.PRESS or action == glfw.REPEAT:
                print("BOOM!!")

    def resize_callback(self, window, width, height):
        self.engine.resize(width, height)


if __name__ == "__main__":

    if len(sys.argv) > 1:
        morphoRender = MorphoRender(sys.argv[1])
        morphoRender.run()
