# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import copy
import glfw
from numpy import float32
from render.render import *
from morphology import *
import sys
import os


class MorphoRender:

    def __init__(self, paths):
        self.width = 600
        self.height = 600

        if not self.__init_window():
            return None
        glfw.set_key_callback(self.window, self.key_callback)
        glfw.set_window_size_callback(self.window, self.resize_callback)
        glfw.make_context_current(self.window)
        print(glGetString(GL_VERSION))

        self.background_color = Vec3(1)
        self.engine = Engine(self.width, self.height)

        for path in paths:
            filename, file_extension = os.path.splitext(path)
            if file_extension == ".swc":
                self.engine.scene.meshes.append(
                    mesh_from_morpho(load_swc(path)))
            elif file_extension == ".obj":
                self.engine.scene.meshes.append(load_obj(path))

    def __init_window(self):
        if not glfw.init():
            return False
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 1)
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
            self.engine.render(t)
            t += 0.01
            glfw.swap_buffers(self.window)
            glfw.poll_events()
        glfw.terminate()

    def key_callback(self, window, key, scancode, action, mods):

        if action == glfw.PRESS or action == glfw.REPEAT:
            if key == glfw.KEY_D:
                self.engine.scene.camera.position += Vec3(0.1, 0, 0)
            elif key == glfw.KEY_A:
                self.engine.scene.camera.position += Vec3(-0.1, 0, 0)
            elif key == glfw.KEY_W:
                self.engine.scene.camera.position += Vec3(0.0, 0, -0.1)
            elif key == glfw.KEY_S:
                self.engine.scene.camera.position += Vec3(0.0, 0, 0.1)
            elif key == glfw.KEY_SPACE:
                self.engine.render_mode()

    def resize_callback(self, window, width, height):
        glfw.make_context_current(self.window)
        self.engine.resize(width, height)


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("Error. Use: " + sys.argv[0] + " file[.swc]")
        exit()
    morphoRender = MorphoRender(sys.argv[1:])

    morphoRender.run()
