# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import copy
from math import ceil
from threading import currentThread
import glfw
from numpy import float32
from render.render import *
from data.morphology import *
from data.loaders import *
import sys
import os
import time


class MorphoRender:

    def __init__(self, paths):
        self.width = 600
        self.height = 600

        self.translate = False
        self.rotate = False
        self.x = 0
        self.y = 0

        if not self.__init_window():
            return None
        glfw.set_key_callback(self.window, self.key_callback)
        glfw.set_window_size_callback(self.window, self.resize_callback)
        glfw.set_scroll_callback(self.window, self.scroll_callback)
        glfw.set_mouse_button_callback(self.window, self.mouse_button_callback)
        glfw.set_cursor_pos_callback(
            self.window, self.cursor_position_callback)

        glfw.make_context_current(self.window)
        print(glGetString(GL_VERSION))

        self.background_color = Vec3(1)
        self.scene = Scene()

        # Shaders init

        program_triangles = ShaderProgram([("shaders/triangles.vert", ShaderType.VERTEX),
                                          ("shaders/triangles.frag", ShaderType.FRAGMENT)])
        program_lines = ShaderProgram([("shaders/lines.vert", ShaderType.VERTEX),
                                       ("shaders/lines.frag", ShaderType.FRAGMENT)])
        program_background = ShaderProgram([("shaders/background.vert", ShaderType.VERTEX),
                                            ("shaders/background.frag", ShaderType.FRAGMENT)])

        self.scene.add_model((Quad(), program_background, Mat4()))

        total_paths = float(len(paths))
        num_paths = 0
        mesh_loaded = 0
        displaceX = 0.0
        displaceY = 0.0
        meshes_per_row = ceil(math.sqrt(len(paths)))
        for path in paths:
            num_paths += 1
            filename, file_extension = os.path.splitext(path)
            if file_extension == ".swc":
                mesh = mesh_from_morpho(load_swc(path))
                displaceX += mesh.aabb.radius()
                self.scene.add_model((mesh, program_triangles,
                                      Mat4([1, 0, 0, displaceX,
                                            0, 1, 0, displaceY-mesh.aabb.radius(),
                                            0, 0, 1, 0,
                                            0, 0, 0, 1])))
                displaceX += mesh.aabb.radius()
                mesh_loaded += 1

                mesh = lines_from_morpho(load_swc(path))
                displaceX += mesh.aabb.radius()
                self.scene.add_model((mesh, program_lines,
                                      Mat4([1, 0, 0, displaceX,
                                            0, 1, 0, displaceY-mesh.aabb.radius(),
                                            0, 0, 1, 0,
                                            0, 0, 0, 1])))
                displaceX += mesh.aabb.radius()
                mesh_loaded += 1
            elif file_extension == ".obj":
                mesh = load_obj(path)
                # mesh = Quad()
                if mesh is not None:
                    displaceX += mesh.aabb.radius()
                    self.scene.add_model((mesh, program_triangles,
                                          Mat4([1, 0, 0, displaceX,
                                                0, 1, 0, displaceY-mesh.aabb.radius(),
                                                0, 0, 1, 0,
                                                0, 0, 0, 1])))

                    displaceX += mesh.aabb.radius()
                    mesh_loaded += 1
            if mesh_loaded % meshes_per_row == 0:
                displaceY = self.scene.aabb.min.y
                displaceX = 0.0
            print("\rLoading meshes " +
                  str(int(num_paths/total_paths*100))+"%", end='')

        print()
        num_triangles = 0
        for model in self.scene.models:
            num_triangles += model[0].num_triangles/3
        print("Loaded " + str(mesh_loaded) + " meshes with " +
              str(num_triangles/1000.0) + "K triangles")

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
        fps = 0
        prev_time = time.time()
        while not glfw.window_should_close(self.window):
            self.scene.render()
            fps += 1
            current_time = time.time()
            step = current_time - prev_time
            if step > 1.0:
                prev_time = current_time
                print("\rFPS: " + str(int(fps/step)) + "    ", end='')
                fps = 0
            glfw.swap_buffers(self.window)
            glfw.poll_events()
        glfw.terminate()

    def key_callback(self, window, key, scancode, action, mods):

        if action == glfw.PRESS or action == glfw.REPEAT:
            r = self.scene.radius * 0.05
            if key == glfw.KEY_D:
                self.scene.target += Vec3(r, 0, 0)
            elif key == glfw.KEY_A:
                self.scene.target += Vec3(-r, 0, 0)
            elif key == glfw.KEY_W:
                self.scene.target += Vec3(0.0, 0, -r)
            elif key == glfw.KEY_S:
                self.scene.target += Vec3(0.0, 0, r)
            elif key == glfw.KEY_M:
                self.scene.render_mode()

    def scroll_callback(self, window, x, y):
        if (y > 0):
            self.scene.radius *= 0.9
        else:
            self.scene.radius *= 1.1

    def mouse_button_callback(self, window, button, action, mods):
        if button == glfw.MOUSE_BUTTON_MIDDLE:
            if action == glfw.PRESS:
                self.translate = True
                (self.x, self.y) = glfw.get_cursor_pos(window)
            else:
                self.translate = False
        elif button == glfw.MOUSE_BUTTON_RIGHT:
            if action == glfw.PRESS:
                self.rotate = True
                (self.x, self.y) = glfw.get_cursor_pos(window)
            else:
                self.rotate = False

    def cursor_position_callback(self, window, x, y):
        x_diff = x - self.x
        y_diff = y - self.y
        r = self.scene.radius * 0.005
        if self.translate:
            self.scene.target += Vec3(-r*x_diff, r*y_diff, 0)
        elif self.rotate:
            rot = rotation_from_yaw_pitch(x_diff*0.01, y_diff*0.01)
            self.scene.direction = rot * self.scene.direction

        self.x = x
        self.y = y

    def resize_callback(self, window, width, height):
        glfw.make_context_current(self.window)
        self.scene.ratio = float(width)/height
        glViewport(0, 0, width, height)


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("Error. Use: " + sys.argv[0] + " file[.swc]")
        exit()
    morphoRender = MorphoRender(sys.argv[1:])

    morphoRender.run()
